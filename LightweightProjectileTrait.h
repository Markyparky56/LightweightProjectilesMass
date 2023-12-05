// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MassEntityTraitBase.h"
#include "Mass/ProjectileFragments.h"
#include "LightweightProjectileTrait.generated.h"

/**
 * 
 */
UCLASS()
class LYRAGAME_API ULightweightProjectileTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()
	
public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& buildContext, const UWorld& world) const override;

	UPROPERTY(EditAnywhere, meta = (ShowOnlyInnerProperties))
	FProjectileArchetypeDescription ProjectileArchetypeDescription;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere)
	float GravityScale = 1.f;

	// TODO: Consider how to handle switching between a single-hit projectile and once that handles shot penetration with multi sweeps

};
