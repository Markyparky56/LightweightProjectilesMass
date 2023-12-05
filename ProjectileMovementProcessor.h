// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "ProjectileMovementProcessor.generated.h"

/**
 * 
 */
UCLASS()
class LYRAGAME_API UProjectileMovementProcessor : public UMassProcessor
{
	GENERATED_BODY()
public:
	UProjectileMovementProcessor();

	static const FName ProjectileEntityHitSignal;

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& entityManager, FMassExecutionContext& context) override;

	FMassEntityQuery ProjectileMovementQuery;

};
