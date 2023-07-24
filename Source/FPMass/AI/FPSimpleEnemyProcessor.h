#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassProcessor.h"
#include "FPMass/ISMRepresentation/FPISMMisc.h"

#include "FPSimpleEnemyProcessor.generated.h"

UENUM()
enum class EFPSimpleEnemyState : uint8
{
	Idle,
	Moving,
	Attacking
};

USTRUCT(BlueprintType)
struct FPMASS_API FFPSimpleEnemyStateFragment : public FMassFragment
{
	GENERATED_BODY()

	EFPSimpleEnemyState State = EFPSimpleEnemyState::Idle;

	float LastMoved = 0.0f;
};

USTRUCT(BlueprintType)
struct FPMASS_API FFPSimpleEnemyParameters : public FMassSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayAbility> AttackAbility;

	UPROPERTY(EditAnywhere, meta=(UIMin = "2", UIMax = "1000"))
	int AttackAnimationIndex = 2; // 0 = idle, 1 = walk
};

UCLASS()
class FPMASS_API UFPSimpleEnemyProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UFPSimpleEnemyProcessor();

	virtual void ConfigureQueries() override;

	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery EntityQuery;
	FMassEntityQuery PushArraysToNiagaraSystems;
};
