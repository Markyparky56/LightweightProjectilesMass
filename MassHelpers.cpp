// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/MassHelpers.h"
#include "Algo/Transform.h"
#include "MassCommonFragments.h"
#include "MassEntityConfigAsset.h"
#include "MassEntitySubsystem.h"
#include "MassSpawnerSubsystem.h"
#include "MassMovementFragments.h"
#include "Mass/ProjectileFragments.h"

namespace Algo {
	static bool IsValid(const UObject* test)
	{
		return ::IsValid(test);
	}
}

/*static*/ FMassEntityViewWrapper UMassHelpers::BP_SpawnEntityFromEntityConfig(const UObject* worldContextObject, UMassEntityConfigAsset* massEntityConfig, EMassHelpersReturnSuccess& returnBranch)
{
	FMassEntityViewWrapper outView;
	if (!IsValid(worldContextObject))
	{
		returnBranch = EMassHelpersReturnSuccess::Failure;
		return outView;
	}

	const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(world))
	{
		returnBranch = EMassHelpersReturnSuccess::Failure;
		return outView;
	}
	outView = SpawnEntityFromEntityConfig(world, massEntityConfig);
	if (outView.EntityView.IsSet())
	{
		returnBranch = EMassHelpersReturnSuccess::Success;
	}
	return outView;
}

FMassEntityViewWrapper UMassHelpers::SpawnEntityFromEntityConfig(const UWorld* world, UMassEntityConfigAsset* massEntityConfig)
{
	FMassEntityViewWrapper outView;
	if (!IsValid(massEntityConfig))
	{
		return outView;
	}

	const FMassEntityTemplate& entityTemplate = massEntityConfig->GetConfig().GetOrCreateEntityTemplate(*world);
	UMassSpawnerSubsystem* spawnerSS = world->GetSubsystem<UMassSpawnerSubsystem>();
	UMassEntitySubsystem* entitySS = world->GetSubsystem<UMassEntitySubsystem>();
	FMassEntityManager& entityManager = entitySS->GetMutableEntityManager();

	// Ideally this would be an inline allocator but SpawnEntities needs templated for that
	TArray<FMassEntityHandle> entities;
	spawnerSS->SpawnEntities(entityTemplate, 1, entities);
	if (!ensure(entities.Num() != 0))
	{
		return outView;
	}

	outView = FMassEntityViewWrapper(entityManager, entities[0]);
	return outView;
}

void UMassHelpers::DestroyEntity_View(const UObject* worldContextObject, FMassEntityViewWrapper entity)
{
	DestroyEntity_Handle(worldContextObject, entity.EntityView.GetEntity());
}

void UMassHelpers::DestroyEntity_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity)
{
	const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(world))
	{
		return;
	}

	UMassEntitySubsystem* entitySS = world->GetSubsystem<UMassEntitySubsystem>();
	FMassEntityManager& entityManager = entitySS->GetMutableEntityManager();

	if (entity.Handle.IsValid())
	{
		entityManager.DestroyEntity(entity.Handle);
	}
	else
	{
		entityManager.ReleaseReservedEntity(entity.Handle);
	}
}

bool UMassHelpers::EntityHasFragment_View(FMassEntityViewWrapper entity, FInstancedStruct fragment)
{
	if (fragment.IsValid() && fragment.GetScriptStruct()->IsChildOf(FMassFragment::StaticStruct()))
	{
		return entity.EntityView.GetFragmentDataStruct(fragment.GetScriptStruct()).IsValid();
	}
	return false;
}

bool UMassHelpers::EntityHasFragment_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, FInstancedStruct fragment)
{
	const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(world))
	{
		return false;
	}
	const FMassEntityManager& entityManager = world->GetSubsystem<UMassEntitySubsystem>()->GetEntityManager();
	FMassEntityView view(entityManager, entity);

	return EntityHasFragment_View(view, fragment);
}

bool UMassHelpers::EntityHasTag_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, FInstancedStruct tag)
{	
	return EntityHasTag_Handle(worldContextObject, entity.EntityView.GetEntity(), tag);
}

bool UMassHelpers::EntityHasTag_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, FInstancedStruct tag)
{
	const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(world))
	{
		return false;
	}

	if (tag.IsValid() && tag.GetScriptStruct()->IsChildOf(FMassTag::StaticStruct()))
	{
		// Need to check the archetype since tags are special
		const FMassEntityManager& entityManager = world->GetSubsystem<UMassEntitySubsystem>()->GetEntityManager();
		FMassArchetypeHandle archetype = entityManager.GetArchetypeForEntity(entity.Handle);
		return entityManager.GetArchetypeComposition(archetype).Tags.Contains(*tag.GetScriptStruct());
	}
	return false;
}

bool UMassHelpers::IsEntityValid_View(const UObject* worldContextObject, FMassEntityViewWrapper entity)
{
	// TODO: Look at if these are equivalent
	//return entity.EntityView.IsSet
	return IsEntityValid_Handle(worldContextObject, entity.EntityView.GetEntity());
}

bool UMassHelpers::IsEntityValid_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity)
{
	if (const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		const FMassEntityManager& entityManager = world->GetSubsystem<UMassEntitySubsystem>()->GetEntityManager();
		return entityManager.IsEntityValid(entity.Handle);
	}	
	return false;
}

void UMassHelpers::SetEntityTransform_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, const FTransform& transform)
{
	if (!IsEntityValid_View(worldContextObject, entity))
	{
		return;
	}

	if (FTransformFragment* transformFragment = entity.EntityView.GetFragmentDataPtr<FTransformFragment>())
	{
		transformFragment->SetTransform(transform);
	}
}

void UMassHelpers::SetEntityTransform_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, const FTransform& transform)
{
	if (!IsEntityValid_Handle(worldContextObject, entity))
	{
		return;
	}

	if (const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		const FMassEntityManager& entityManager = world->GetSubsystem<UMassEntitySubsystem>()->GetEntityManager();
		if (FTransformFragment* transformFragment = entityManager.GetFragmentDataPtr<FTransformFragment>(entity.Handle))
		{
			transformFragment->SetTransform(transform);
		}
	}
}

void UMassHelpers::GetEntityTransform_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, FTransform& transform)
{
	if (!IsEntityValid_View(worldContextObject, entity))
	{
		return;
	}

	if (const FTransformFragment* transformFragment = entity.EntityView.GetFragmentDataPtr<FTransformFragment>())
	{
		transform = transformFragment->GetTransform();
	}
}

void UMassHelpers::GetEntityTransform_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, FTransform& transform)
{
	if (!IsEntityValid_Handle(worldContextObject, entity))
	{
		return;
	}

	if (const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		const FMassEntityManager& entityManager = world->GetSubsystem<UMassEntitySubsystem>()->GetEntityManager();
		if (const FTransformFragment* transformFragment = entityManager.GetFragmentDataPtr<FTransformFragment>(entity.Handle))
		{
			transform = transformFragment->GetTransform();
		}
	}
}

void UMassHelpers::SetEntityVelocity_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, const FVector& velocity)
{
	if (!IsEntityValid_View(worldContextObject, entity))
	{
		return;
	}

	if (FMassVelocityFragment* velocityFragment = entity.EntityView.GetFragmentDataPtr<FMassVelocityFragment>())
	{
		velocityFragment->Value = velocity;
	}
}

void UMassHelpers::SetEntityVelocity_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, const FVector& velocity)
{
	if (!IsEntityValid_Handle(worldContextObject, entity))
	{
		return;
	}

	if (const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		const FMassEntityManager& entityManager = world->GetSubsystem<UMassEntitySubsystem>()->GetEntityManager();
		if (FMassVelocityFragment* velocityFragment = entityManager.GetFragmentDataPtr<FMassVelocityFragment>(entity.Handle))
		{
			velocityFragment->Value = velocity;
		}
	}
}

void UMassHelpers::GetEntityVelocity_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, FVector& velocity)
{
	if (!IsEntityValid_View(worldContextObject, entity))
	{
		return;
	}

	if (const FMassVelocityFragment* velocityFragment = entity.EntityView.GetFragmentDataPtr<FMassVelocityFragment>())
	{
		velocity = velocityFragment->Value;
	}
}

void UMassHelpers::GetEntityVelocity_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, FVector& velocity)
{
	if (!IsEntityValid_Handle(worldContextObject, entity))
	{
		return;
	}

	if (const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		const FMassEntityManager& entityManager = world->GetSubsystem<UMassEntitySubsystem>()->GetEntityManager();
		if (const FMassVelocityFragment* velocityFragment = entityManager.GetFragmentDataPtr<FMassVelocityFragment>(entity.Handle))
		{
			velocity = velocityFragment->Value;
		}
	}
}

void UMassHelpers::SetEntityForce_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, const FVector& force)
{
	if (!IsEntityValid_View(worldContextObject, entity))
	{
		return;
	}

	if (FMassForceFragment* forceFragment = entity.EntityView.GetFragmentDataPtr<FMassForceFragment>())
	{
		forceFragment->Value = force;
	}
}

void UMassHelpers::SetEntityForce_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, const FVector& force)
{
	if (!IsEntityValid_Handle(worldContextObject, entity))
	{
		return;
	}

	if (const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		const FMassEntityManager& entityManager = world->GetSubsystem<UMassEntitySubsystem>()->GetEntityManager();
		if (FMassForceFragment* forceFragment = entityManager.GetFragmentDataPtr<FMassForceFragment>(entity.Handle))
		{
			forceFragment->Value = force;
		}
	}
}

void UMassHelpers::GetEntityForce_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, FVector& force)
{
	if (!IsEntityValid_View(worldContextObject, entity))
	{
		return;
	}

	if (const FMassForceFragment* forceFragment = entity.EntityView.GetFragmentDataPtr<FMassForceFragment>())
	{
		force = forceFragment->Value;
	}
}

void UMassHelpers::GetEntityForce_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, FVector& force)
{
	if (!IsEntityValid_Handle(worldContextObject, entity))
	{
		return;
	}

	if (const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		const FMassEntityManager& entityManager = world->GetSubsystem<UMassEntitySubsystem>()->GetEntityManager();
		if (const FMassForceFragment* forceFragment = entityManager.GetFragmentDataPtr<FMassForceFragment>(entity.Handle))
		{
			force = forceFragment->Value;
		}
	}
}

void UMassHelpers::SetEntityInstigatorOwner_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, AActor* instigator, AActor* owner)
{
	if (!IsEntityValid_View(worldContextObject, entity))
	{
		return;
	}

	if (FInstigatorOwnerFragment* instigatorOwner = entity.EntityView.GetFragmentDataPtr<FInstigatorOwnerFragment>())
	{
		instigatorOwner->InstigatorActor = instigator;
		instigatorOwner->Owner = owner;
	}
}

void UMassHelpers::SetEntityInstigatorOwner_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, AActor* instigator, AActor* owner)
{
	if (!IsEntityValid_Handle(worldContextObject, entity))
	{
		return;
	}

	if (const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		const FMassEntityManager& entityManager = world->GetSubsystem<UMassEntitySubsystem>()->GetEntityManager();
		if (FInstigatorOwnerFragment* instigatorOwner = entityManager.GetFragmentDataPtr<FInstigatorOwnerFragment>(entity.Handle))
		{
			instigatorOwner->InstigatorActor = instigator;
			instigatorOwner->Owner = owner;
		}
	}
}

AActor* UMassHelpers::GetEntityInstigator_View(const UObject* worldContextObject, FMassEntityViewWrapper entity)
{
	if (IsEntityValid_View(worldContextObject, entity))
	{
		if (const FInstigatorOwnerFragment* instigatorOwner = entity.EntityView.GetFragmentDataPtr<FInstigatorOwnerFragment>())
		{
			return instigatorOwner->InstigatorActor.Get();
		}
	}
	return nullptr;
}

AActor* UMassHelpers::GetEntityInstigator_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity)
{
	if (IsEntityValid_Handle(worldContextObject, entity))
	{
		if (const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			const FMassEntityManager& entityMangaer = world->GetSubsystem<UMassEntitySubsystem>()->GetEntityManager();
			if (const FInstigatorOwnerFragment* instigatorOwner = entityMangaer.GetFragmentDataPtr<FInstigatorOwnerFragment>(entity.Handle))
			{
				return instigatorOwner->InstigatorActor.Get();
			}
		}
	}
	return nullptr;
}

AActor* UMassHelpers::GetEntityOwner_View(const UObject* worldContextObject, FMassEntityViewWrapper entity)
{
	if (IsEntityValid_View(worldContextObject, entity))
	{
		if (const FInstigatorOwnerFragment* instigatorOwner = entity.EntityView.GetFragmentDataPtr<FInstigatorOwnerFragment>())
		{
			return instigatorOwner->Owner.Get();
		}
	}
	return nullptr;
}

AActor* UMassHelpers::GetEntityOwner_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity)
{
	if (IsEntityValid_Handle(worldContextObject, entity))
	{
		if (const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			const FMassEntityManager& entityMangaer = world->GetSubsystem<UMassEntitySubsystem>()->GetEntityManager();
			if (const FInstigatorOwnerFragment* instigatorOwner = entityMangaer.GetFragmentDataPtr<FInstigatorOwnerFragment>(entity.Handle))
			{
				return instigatorOwner->Owner.Get();
			}
		}
	}
	return nullptr;
}

void UMassHelpers::SetEntityIgnoredActors_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, const TArray<AActor*>& ignoredActors)
{
	if (!IsEntityValid_View(worldContextObject, entity))
	{
		return;
	}

	if (FCollisionIgnoredFragment* collisionIgnored = entity.EntityView.GetFragmentDataPtr<FCollisionIgnoredFragment>())
	{
		collisionIgnored->IgnoredActors.Empty(ignoredActors.Num());
		Algo::TransformIf(ignoredActors, collisionIgnored->IgnoredActors, &Algo::IsValid, FIdentityFunctor());
	}
}

void UMassHelpers::SetEntityIgnoredActors_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, const TArray<AActor*>& ignoredActors)
{
	if (!IsEntityValid_Handle(worldContextObject, entity))
	{
		return;
	}

	if (const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		const FMassEntityManager& entityManager = world->GetSubsystem<UMassEntitySubsystem>()->GetEntityManager();
		if (FCollisionIgnoredFragment* collisionIgnored = entityManager.GetFragmentDataPtr<FCollisionIgnoredFragment>(entity.Handle))
		{
			collisionIgnored->IgnoredActors.Empty(ignoredActors.Num());
			Algo::TransformIf(ignoredActors, collisionIgnored->IgnoredActors, &Algo::IsValid, FIdentityFunctor());
		}
	}
}

void UMassHelpers::SetEntityIgnoredComponents_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, const TArray<UPrimitiveComponent*>& ignoredComps)
{
	if (!IsEntityValid_View(worldContextObject, entity))
	{
		return;
	}

	if (FCollisionIgnoredFragment* collisionIgnored = entity.EntityView.GetFragmentDataPtr<FCollisionIgnoredFragment>())
	{
		collisionIgnored->IgnoredComponents.Empty(ignoredComps.Num());
		Algo::TransformIf(ignoredComps, collisionIgnored->IgnoredComponents, &Algo::IsValid, FIdentityFunctor());
	}
}

void UMassHelpers::SetEntityIgnoredComponents_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, const TArray<UPrimitiveComponent*>& ignoredComps)
{
	if (!IsEntityValid_Handle(worldContextObject, entity))
	{
		return;
	}

	if (const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		const FMassEntityManager& entityManager = world->GetSubsystem<UMassEntitySubsystem>()->GetEntityManager();
		if (FCollisionIgnoredFragment* collisionIgnored = entityManager.GetFragmentDataPtr<FCollisionIgnoredFragment>(entity.Handle))
		{
			collisionIgnored->IgnoredComponents.Empty(ignoredComps.Num());
			Algo::TransformIf(ignoredComps, collisionIgnored->IgnoredComponents, &Algo::IsValid, FIdentityFunctor());
		}
	}
}

void UMassHelpers::GetEntityIgnoredActors_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, TArray<AActor*>& ignoredActors)
{
	if (IsEntityValid_View(worldContextObject, entity))
	{
		if (const FCollisionIgnoredFragment* collisionIgnored = entity.EntityView.GetFragmentDataPtr<FCollisionIgnoredFragment>())
		{
			ignoredActors.Empty(collisionIgnored->IgnoredActors.Num());
			Algo::TransformIf(collisionIgnored->IgnoredActors, ignoredActors, 
				[](const TWeakObjectPtr<AActor>& actor) -> bool { return actor.IsValid(); },
				[](const TWeakObjectPtr<AActor>& actor) -> AActor* { return actor.Get(); });
		}
	}
}

void UMassHelpers::GetEntityIgnoredActors_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, TArray<AActor*>& ignoredActors)
{
	if (IsEntityValid_Handle(worldContextObject, entity))
	{
		if (const UWorld* world = GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			const FMassEntityManager& entityManager = world->GetSubsystem<UMassEntitySubsystem>()->GetEntityManager();
			if (FCollisionIgnoredFragment* collisionIgnored = entityManager.GetFragmentDataPtr<FCollisionIgnoredFragment>(entity.Handle))
			{

			}
		}
	}
}

void UMassHelpers::GetEntityIgnoredComponents_View(const UObject* worldContextObject, FMassEntityViewWrapper entity, TArray<UPrimitiveComponent*>& ignoredActors)
{
}

void UMassHelpers::GetEntityIgnoredComponents_Handle(const UObject* worldContextObject, FMassEntityHandleWrapper entity, TArray<UPrimitiveComponent*>& ignoredActors)
{
}
