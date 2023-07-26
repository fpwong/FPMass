// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "UObject/Object.h"
#include "FPSlideMovementProcessor.generated.h"

UCLASS()
class FPMASS_API UFPSlideMovementProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UFPSlideMovementProcessor();

	FMassEntityQuery MovementEntityQuery;

	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
};


/**
 * Helper class for slide collision against the world
 * Credits: https://blog.littlepolygon.com/posts/sliding/
 */
class FSlide
{
public:
	// out start position
	FVector Position = FVector(0.0f);

	// the desired offset vector
	FVector Remainder = FVector(0.0f);

	// the shape to slide
	FCollisionShape Shape = FCollisionShape::MakeSphere(20.0f);

	// the rotation of the shape
	FQuat Rotation = FQuat::Identity;

	// parameters used by unreal's collision-query system
	FCollisionQueryParams QueryParams = FCollisionQueryParams::DefaultQueryParam;
	ECollisionChannel Channel = ECC_Pawn;

	// Advance Position/Remainder to the next impact
	bool TryStep(const UWorld* World, FHitResult& Hit);

	FVector PullBackMove(const FVector& Move);
	FCollisionShape InflateShape(const FCollisionShape& InShape, float Amount);
	bool DepenAndSweep(const UWorld* World, FHitResult& Hit, const FVector& Start, const FVector& Delta, const FQuat& Rot, ECollisionChannel InChannel, const FCollisionShape& InShape, const FCollisionQueryParams& Params);

private:
	FVector PrevNormal = FVector(0.0f);
	TArray<FHitResult> Hits;
};