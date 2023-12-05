// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/LightweightProjectileTrait.h"
#include "MassCommonFragments.h"
#include "MassEntitySubsystem.h"
#include "MassEntityTemplateRegistry.h"
#include "MassMovementFragments.h"

void ULightweightProjectileTrait::BuildTemplate(FMassEntityTemplateBuildContext& buildContext, const UWorld& world) const
{
	UMassEntitySubsystem* entitySS = world.GetSubsystem<UMassEntitySubsystem>();
	FMassEntityManager& entityManager = entitySS->GetMutableEntityManager();

	buildContext.AddFragment<FTransformFragment>();
	buildContext.AddFragment<FMassForceFragment>();
	buildContext.AddFragment<FMassVelocityFragment>();

	FConstSharedStruct archetypeDescFrag = entityManager.GetOrCreateConstSharedFragment<FProjectileArchetypeDescription>(ProjectileArchetypeDescription);
	buildContext.AddConstSharedFragment(archetypeDescFrag);

	FGEDamageFragment damageFragment;
	damageFragment.DamageEffect = DamageEffect;
	FConstSharedStruct damageFrag = entityManager.GetOrCreateConstSharedFragment<FGEDamageFragment>(damageFragment);
	buildContext.AddConstSharedFragment(damageFrag);

	buildContext.AddFragment<FInstigatorOwnerFragment>();
	buildContext.AddFragment<FHitInfoFragment>();
	buildContext.AddFragment<FCollisionIgnoredFragment>();

	FGravityScaleFragment gravityScaleFragment;
	gravityScaleFragment.GravityScale = GravityScale;
	FConstSharedStruct gravityScaleFrag = entityManager.GetOrCreateConstSharedFragment<FGravityScaleFragment>(gravityScaleFragment);
	buildContext.AddConstSharedFragment(gravityScaleFrag);
}
