// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityView.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MassHelpers.generated.h"

class UMassEntityConfigAsset;

// Not for storing, transient handle to an entity within an archetype 
// Guaranteed to only be valid within the scope it was created
USTRUCT(BlueprintType, meta=(DisplayName="Mass Entity View"))
struct FMassEntityViewWrapper
{
	GENERATED_BODY()

	FMassEntityViewWrapper() = default;
	FMassEntityViewWrapper(const FMassEntityView& view) : EntityView(view) {}
	FMassEntityViewWrapper(const FMassArchetypeHandle& archetype, FMassEntityHandle entityHandle)
		: EntityView(archetype, entityHandle)
	{}
	FMassEntityViewWrapper(const FMassEntityManager& manager, FMassEntityHandle entityHandle)
		: EntityView(manager, entityHandle)
	{}
	operator FMassEntityView() const { return EntityView; }

	FMassEntityView EntityView;
};

// Safe to store
USTRUCT(BlueprintType, meta=(DisplayName="Mass Entity Handle"))
struct FMassEntityHandleWrapper
{
	GENERATED_BODY()

	FMassEntityHandleWrapper() = default;
	FMassEntityHandleWrapper(FMassEntityHandle entityHandle)
		: Handle(entityHandle)
	{}
	operator FMassEntityHandle() const { return Handle; }

	FMassEntityHandle Handle;
};

UENUM()
enum class EMassHelpersReturnSuccess : uint8
{
	Success,
	Failure
};

UCLASS()
class LYRAGAME_API UMassHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "Mass Helpers", meta = (DisplayName = "Spawn Entity From Entity Config", WorldContext = "worldContextObject", ExpandEnumAsExecs = "returnBranch"))
	static FMassEntityViewWrapper BP_SpawnEntityFromEntityConfig(const UObject* worldContextObject, UMassEntityConfigAsset* massEntityConfig, EMassHelpersReturnSuccess& returnBranch);
	static FMassEntityViewWrapper SpawnEntityFromEntityConfig(const UWorld* world, UMassEntityConfigAsset* massEntityConfig);

	UFUNCTION(BlueprintCallable, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Destroy Entity (view)"))
	static void DestroyEntity_View(const UObject* worldContextObject, FMassEntityViewWrapper entity);
	UFUNCTION(BlueprintCallable, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Destroy Entity (handle)"))
	static void DestroyEntity_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity);

	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (DisplayName = "Entity Has Fragment (view)"))
	static bool EntityHasFragment_View(FMassEntityViewWrapper entity, FInstancedStruct fragment);
	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext="worldContextObject", DisplayName = "Entity Has Fragment (handle)"))
	static bool EntityHasFragment_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, FInstancedStruct fragment);

	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta=(WorldContext = "worldContextObject", DisplayName = "Entity Has Tag (view)"))
	static bool EntityHasTag_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, FInstancedStruct tag);
	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Entity Has Tag (handle)"))
	static bool EntityHasTag_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, FInstancedStruct tag);

	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta=(WorldContext = "worldContextObject", DisplayName = "Is Entity Valid (view)"))
	static bool IsEntityValid_View(const UObject* worldContextObject, FMassEntityViewWrapper entity);
	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Is Entity Valid (handle)"))
	static bool IsEntityValid_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity);

	UFUNCTION(BlueprintCallable, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Set Entity Transform (view)", AutoCreateRefTerm="transform"))
	static void SetEntityTransform_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, const FTransform& transform);
	UFUNCTION(BlueprintCallable, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Set Entity Transform (handle)", AutoCreateRefTerm="trasnform"))
	static void SetEntityTransform_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, const FTransform& transform);
	
	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Get Entity Transform (view)"))
	static void GetEntityTransform_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, FTransform& transform);
	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Get Entity Transform (handle)"))
	static void GetEntityTransform_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, FTransform& transform);

	UFUNCTION(BlueprintCallable, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Set Entity Velocity (view)", AutoCreateRefTerm = "velocity"))
	static void SetEntityVelocity_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, const FVector& velocity);
	UFUNCTION(BlueprintCallable, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Set Entity Velocity (handle)", AutoCreateRefTerm = "velocity"))
	static void SetEntityVelocity_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, const FVector& velocity);

	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Get Entity Velocity (view)"))
	static void GetEntityVelocity_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, FVector& velocity);
	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Get Entity Velocity (handle)"))
	static void GetEntityVelocity_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, FVector& velocity);

	UFUNCTION(BlueprintCallable, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Set Entity Force (view)", AutoCreateRefTerm = "force"))
	static void SetEntityForce_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, const FVector& force);
	UFUNCTION(BlueprintCallable, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Set Entity Force (handle)", AutoCreateRefTerm = "force"))
	static void SetEntityForce_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, const FVector& force);

	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Get Entity Force (view)"))
	static void GetEntityForce_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, FVector& force);
	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Get Entity Force (handle)"))
	static void GetEntityForce_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, FVector& force);

	UFUNCTION(BlueprintCallable, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Set Entity Instigator/Owner (view)", AutoCreateRefTerm = "force"))
	static void SetEntityInstigatorOwner_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, AActor* instigator, AActor* owner);
	UFUNCTION(BlueprintCallable, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Set Entity Instigator/Owner (handle)", AutoCreateRefTerm = "force"))
	static void SetEntityInstigatorOwner_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, AActor* instigator, AActor* owner);

	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Get Entity Instigator (view)"))
	static AActor* GetEntityInstigator_View(const UObject* worldContextObject, FMassEntityViewWrapper entity);
	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Get Entity Instigator (handle)"))
	static AActor* GetEntityInstigator_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity);
	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Get Entity Owner (view)"))
	static AActor* GetEntityOwner_View(const UObject* worldContextObject, FMassEntityViewWrapper entity);
	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Get Entity Owner (handle)"))
	static AActor* GetEntityOwner_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity);

	// TODO: Set FCollisionIgnoredFragment
	UFUNCTION(BlueprintCallable, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Set Entity Ignored Actors (view)"))
	static void SetEntityIgnoredActors_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, const TArray<AActor*>& ignoredActors);
	UFUNCTION(BlueprintCallable, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Set Entity Ignored Actors (handle)"))
	static void SetEntityIgnoredActors_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, const TArray<AActor*>& ignoredActors);
	UFUNCTION(BlueprintCallable, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Set Entity Ignored Components (view)"))
	static void SetEntityIgnoredComponents_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, const TArray<UPrimitiveComponent*>& ignoredComps);
	UFUNCTION(BlueprintCallable, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Set Entity Ignored Components (handle)"))
	static void SetEntityIgnoredComponents_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, const TArray<UPrimitiveComponent*>& ignoredComps);

	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Get Entity Ignored Actors (view)"))
	static void GetEntityIgnoredActors_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, TArray<AActor*>& ignoredActors);
	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Get Entity Ignored Actors (handle)"))
	static void GetEntityIgnoredActors_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, TArray<AActor*>& ignoredActors);
	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Get Entity Ignored Components (view)"))
	static void GetEntityIgnoredComponents_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, TArray<UPrimitiveComponent*>& ignoredActors);
	UFUNCTION(BlueprintPure, Category = "Mass Helpers", meta = (WorldContext = "worldContextObject", DisplayName = "Get Entity Ignored Components (handle)"))
	static void GetEntityIgnoredComponents_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, TArray<UPrimitiveComponent*>& ignoredActors);
};
