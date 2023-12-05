// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/ProjectileHitProcessor.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassCommonUtils.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassSignalSubsystem.h"
#include "Mass/ProjectileFragments.h"
#include "Mass/ProjectileMovementProcessor.h"

UProjectileHitProcessor::UProjectileHitProcessor()
{
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;

	ExecutionOrder.ExecuteAfter.Add(UProjectileMovementProcessor::StaticClass()->GetFName());
	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Behavior;

	// Need to talk to GAS
	bRequiresGameThreadExecution = true;
}

void UProjectileHitProcessor::Initialize(UObject& owner)
{
	Super::Initialize(owner);

	UMassSignalSubsystem* signalSS = UWorld::GetSubsystem<UMassSignalSubsystem>(owner.GetWorld());
	SubscribeToSignal(*signalSS, UProjectileMovementProcessor::ProjectileEntityHitSignal);
}

void UProjectileHitProcessor::ConfigureQueries()
{
	// Damage Query
	EntityQuery.AddConstSharedRequirement<FGEDamageFragment>(EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FInstigatorOwnerFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FHitInfoFragment>(EMassFragmentAccess::ReadOnly);

	// TODO: Ricochet Query
}

void UProjectileHitProcessor::SignalEntities(FMassEntityManager& entityManager, FMassExecutionContext& context, FMassSignalNameLookup& entitysignals)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_ProjectileHitProcessor_SignalEntities);

	EntityQuery.ForEachEntityChunk(entityManager, context, [&](FMassExecutionContext& context) {
		QUICK_SCOPE_CYCLE_COUNTER(STAT_ProjectileHitProcessor_ProcessChunk);

		const int32 numEntities = context.GetNumEntities();
		UWorld* world = context.GetWorld();

		// Shared frags
		const FGEDamageFragment& damageFrag = context.GetConstSharedFragment<FGEDamageFragment>();
		TSubclassOf<UGameplayEffect> damageEffect = damageFrag.DamageEffect.Get();

		// Per-entity frags
		TArrayView<const FInstigatorOwnerFragment> instigatorOwnerView = context.GetFragmentView<FInstigatorOwnerFragment>();
		TArrayView<const FHitInfoFragment> hitInfos = context.GetFragmentView<FHitInfoFragment>();

		for (int32 idx = 0; idx < numEntities; ++idx)
		{
			const FInstigatorOwnerFragment& instigatorOwner = instigatorOwnerView[idx];
			const FHitInfoFragment& hitInfo = hitInfos[idx];
			const FHitResult& hit = hitInfo.HitInfo;

			if (AActor* hitActor = hit.GetActor())
			{
				// Long term, this should probably feed data into an ability which can then send it via target data to the server for confirmation

				if (UAbilitySystemComponent* hitASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(hitActor))
				{
					FGameplayEffectContextHandle contextHandle(UAbilitySystemGlobals::Get().AllocGameplayEffectContext());
					FGameplayEffectContext* effectContext = contextHandle.Get();
					effectContext->AddInstigator(instigatorOwner.InstigatorActor.Get(), instigatorOwner.Owner.Get());
					effectContext->AddHitResult(hit);
					effectContext->AddOrigin(hit.TraceStart);

					FGameplayEffectSpec effectSpec(damageEffect.GetDefaultObject(), contextHandle);
					hitASC->ApplyGameplayEffectSpecToSelf(effectSpec);
				}

				DrawDebugPoint(world, hit.ImpactPoint, 10.f, FColor::Red, true);
			}
		}

		// TODO: Filter out entities which can bounce/ricochet
		entityManager.Defer().DestroyEntities(context.GetEntities());
	});
}
