#pragma once

#include "MassCommonTypes.h"
#include "ProjectileFragments.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct LYRAGAME_API FProjectileArchetypeDescription : public FMassSharedFragment
{
	GENERATED_BODY()

	FProjectileArchetypeDescription()
		: CollisionChannel(ECC_Camera)
		, SweepRadius(0.f)
		, bRotationFollowsVelocity(true)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SweepRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bRotationFollowsVelocity : 1;
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FCollisionIgnoredFragment : public FMassFragment
{
	GENERATED_BODY()

	TArray<TWeakObjectPtr<AActor>, TInlineAllocator<2>> IgnoredActors;
	TArray<TWeakObjectPtr<UPrimitiveComponent>, TInlineAllocator<2>> IgnoredComponents;
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FGEDamageFragment : public FMassSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UGameplayEffect> DamageEffect;
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FInstigatorOwnerFragment : public FMassFragment
{
	GENERATED_BODY()

	TWeakObjectPtr<AActor> InstigatorActor; // Pawn or Controller
	TWeakObjectPtr<AActor> Owner; // Damage Causer (pawn)
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FHitInfoFragment : public FMassFragment
{
	GENERATED_BODY()

	FHitResult HitInfo;
};

USTRUCT(BlueprintType)
struct LYRAGAME_API FGravityScaleFragment : public FMassSharedFragment
{
	GENERATED_BODY()

	float GravityScale = 1.f;
};

//USTRUCT(BlueprintType)
//struct LYRAGAME_API FRicochetFragment : public FMassFragment
//{
//	GENERATED_BODY()
//};
//
//USTRUCT(BlueprintType)
//struct LYRAGAME_API FProjectileVisualisationFragment : public FMassSharedFragment
//{
//	GENERATED_BODY()
//};
//
//USTRUCT(BlueprintType)
//struct LYRAGAME_API FHomingTargetFragment : public FMassFragment
//{
//	GENERATED_BODY()
//};
