#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MassSignalProcessorBase.h"
#include "UObject/Object.h"
#include "FPISMAnimationProcessor.generated.h"

UCLASS()
class FPMASS_API UFPISMAnimationProcessors : public UMassProcessor
{
	GENERATED_BODY()

public:
	UFPISMAnimationProcessors();

	virtual void ConfigureQueries() override;

	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery EntityQuery;
};
