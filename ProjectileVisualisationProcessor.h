// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "ProjectileVisualisationProcessor.generated.h"

/**
 * 
 */
UCLASS()
class LYRAGAME_API UProjectileVisualisationProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
protected:
	virtual void ConfigureQueries() override {}
	virtual void Execute(FMassEntityManager& entityManager, FMassExecutionContext& context) override {}

};
