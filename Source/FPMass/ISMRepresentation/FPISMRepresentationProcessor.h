#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MassSignalProcessorBase.h"
#include "UObject/Object.h"
#include "FPISMRepresentationProcessor.generated.h"

UCLASS()
class FPMASS_API UFPISMRepresentationProcessors : public UMassProcessor
{
	GENERATED_BODY()

public:
	UFPISMRepresentationProcessors();

	virtual void ConfigureQueries() override;

	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery PositionToNiagaraFragmentQuery;
};
