#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MassSignalProcessorBase.h"
#include "UObject/Object.h"
#include "FPDebugRepresentationProcessor.generated.h"

USTRUCT()
struct FMassFPDebugRepresentationTag : public FMassTag
{
	GENERATED_BODY()
};

UCLASS()
class FPMASS_API UFPDebugRepresentationProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UFPDebugRepresentationProcessor();

	virtual void ConfigureQueries() override;

	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery EntityQuery;
};
