// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/ProjectileMovementProcessor.h"
#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassCommonUtils.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassSignalSubsystem.h"
#include "Mass/ProjectileFragments.h"

const FName UProjectileMovementProcessor::ProjectileEntityHitSignal = TEXT("ProjectileEntityHitSignal");

UProjectileMovementProcessor::UProjectileMovementProcessor()
{
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;

	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Movement;
}

void UProjectileMovementProcessor::ConfigureQueries()
{
	ProcessorRequirements.AddSubsystemRequirement<UMassSignalSubsystem>(EMassFragmentAccess::ReadWrite);

	// Sweep and behaviour config
	ProjectileMovementQuery.AddConstSharedRequirement<FProjectileArchetypeDescription>(EMassFragmentPresence::All);
	ProjectileMovementQuery.AddConstSharedRequirement<FGravityScaleFragment>(EMassFragmentPresence::All);

	// "Physics" sim
	ProjectileMovementQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	ProjectileMovementQuery.AddRequirement<FMassForceFragment>(EMassFragmentAccess::ReadOnly);
	ProjectileMovementQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
	ProjectileMovementQuery.AddRequirement<FCollisionIgnoredFragment>(EMassFragmentAccess::ReadOnly);

	// Hit output
	ProjectileMovementQuery.AddRequirement<FHitInfoFragment>(EMassFragmentAccess::ReadWrite);
	
	ProjectileMovementQuery.RegisterWithProcessor(*this);
}

void UProjectileMovementProcessor::Execute(FMassEntityManager& entityManager, FMassExecutionContext& context)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_ProjectileMovementProcessor_Execute);

	TQueue<FMassEntityHandle, EQueueMode::Mpsc> entitiesWithHits;
	std::atomic<int32> numEntitiesWithHits = 0;

	// Process entities
	ProjectileMovementQuery.ForEachEntityChunk(entityManager, context, [&](FMassExecutionContext& context) {
		QUICK_SCOPE_CYCLE_COUNTER(STAT_ProjectileMovementProcessor_ProcessChunk);

		const float deltaTime = context.GetDeltaTimeSeconds();
		const int32 numEntities = context.GetNumEntities();

		UWorld* world = context.GetWorld();

		// Shared frags
		const FProjectileArchetypeDescription& archetypeDescription = context.GetConstSharedFragment<FProjectileArchetypeDescription>();
		const FGravityScaleFragment& gravityScale = context.GetConstSharedFragment<FGravityScaleFragment>();

		// Per-entity frags
		TArrayView<FTransformFragment> transforms = context.GetMutableFragmentView<FTransformFragment>();
		TArrayView<const FMassForceFragment> forces = context.GetFragmentView<FMassForceFragment>();
		TArrayView<FMassVelocityFragment> velocities = context.GetMutableFragmentView<FMassVelocityFragment>();
		TArrayView<FHitInfoFragment> hitInfos = context.GetMutableFragmentView<FHitInfoFragment>();
		TArrayView<const FCollisionIgnoredFragment> collisionIgnoredFrags = context.GetFragmentView<FCollisionIgnoredFragment>();

		const float gravityZ = world->GetGravityZ()*gravityScale.GravityScale;
		const FVector gravity(0.f, 0.f, gravityZ);

		FCollisionQueryParams params;
		params.bReturnPhysicalMaterial = true;

		FCollisionShape sweepShape = FCollisionShape::MakeSphere(archetypeDescription.SweepRadius);

		for (int32 idx = 0; idx < numEntities; ++idx)
		{
			FTransform& transform = transforms[idx].GetMutableTransform();
			const FVector& force = forces[idx].Value;
			FVector& velocity = velocities[idx].Value;
			FHitResult& hit = hitInfos[idx].HitInfo;
			const FCollisionIgnoredFragment& ignored = collisionIgnoredFrags[idx];

			const FVector& startPos = transform.GetTranslation();

			// Add force to velocity
			// TODO: handle max speed?
			velocity += force * deltaTime;
			velocity += gravity * deltaTime;

			// Predict end position
			const FVector endPos = startPos + (velocity * deltaTime);

			params.ClearIgnoredActors();
			for (const auto& actor : ignored.IgnoredActors)
				params.AddIgnoredActor(actor.Get());

			params.ClearIgnoredComponents();
			for (const auto& comp : ignored.IgnoredComponents)
				params.AddIgnoredComponent(comp.Get());

			if (world->SweepSingleByChannel(hit, startPos, endPos, transform.GetRotation(), archetypeDescription.CollisionChannel, sweepShape, params))
			{
				// Hit something

				// Not impact point, which is the point on the hit surface the sweep touched
				transform.SetTranslation(hit.Location);

				// Push hit entity
				entitiesWithHits.Enqueue(context.GetEntity(idx));
				++numEntitiesWithHits;
			}
			else // Unblocked movement
			{
				transform.SetTranslation(endPos);
			}

			// I can see why you might want this to be a tag which you run through a second processor, since branching in this loop feels evil
			if (archetypeDescription.bRotationFollowsVelocity)
			{
				transform.SetRotation(velocity.ToOrientationQuat());
			}
		}
	});

	if (numEntitiesWithHits > 0)
	{
		// Signal that we have hits
		TArray<FMassEntityHandle> entities = UE::Mass::Utils::EntityQueueToArray(entitiesWithHits, numEntitiesWithHits);
		context.GetMutableSubsystem<UMassSignalSubsystem>()->SignalEntitiesDeferred(context, ProjectileEntityHitSignal, entities);
	}
}
