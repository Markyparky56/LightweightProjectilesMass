// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassSignalProcessorBase.h"
#include "ProjectileHitProcessor.generated.h"

/**
 * 
 */
UCLASS()
class LYRAGAME_API UProjectileHitProcessor : public UMassSignalProcessorBase//UMassProcessor
{
	GENERATED_BODY()

public:
	UProjectileHitProcessor();

protected:
	virtual void Initialize(UObject& owner) override;
	virtual void ConfigureQueries() override;
	virtual void SignalEntities(FMassEntityManager& entityManager, FMassExecutionContext& context, FMassSignalNameLookup& entitysignals) override;
};
