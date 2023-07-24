﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSimpleOrientation.h"

#include "FPSimpleEnemyProcessor.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "FPMass/AI/FPAIFragments.h"
#include "Kismet/KismetMathLibrary.h"

UFPSimpleOrientationProcessor::UFPSimpleOrientationProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Movement;
}

void UFPSimpleOrientationProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FFPTargetDataFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FFPSimpleEnemyStateFragment>(EMassFragmentAccess::ReadOnly);
}

void UFPSimpleOrientationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	// Clamp max delta time to avoid force explosion on large time steps (i.e. during initialization).
	const float DeltaTime = FMath::Min(0.1f, Context.GetDeltaTimeSeconds());

	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this, DeltaTime](FMassExecutionContext& Context)
	{
		const int32 NumEntities = Context.GetNumEntities();

		const auto& LocationList = Context.GetMutableFragmentView<FTransformFragment>();
		const auto& VelocityList = Context.GetMutableFragmentView<FMassVelocityFragment>();
		const auto& TargetDataList = Context.GetMutableFragmentView<FFPTargetDataFragment>();
		const auto& EnemyStateList = Context.GetMutableFragmentView<FFPSimpleEnemyStateFragment>();

		for (int32 EntityIndex = 0; EntityIndex < NumEntities; ++EntityIndex)
		{
			FTransform& CurrentTransform = LocationList[EntityIndex].GetMutableTransform();
			auto& EnemyState = EnemyStateList[EntityIndex].State;

			switch (EnemyState)
			{
				case EFPSimpleEnemyState::Moving: // rotate to velocity
				{
					// Snap position to move target directly
					FVector Velocity = VelocityList[EntityIndex].Value;
					if (Velocity.SizeSquared2D() >= 50 * 50)
					{
						// UE_LOG(LogTemp, Warning, TEXT("Velocity %f"), Velocity.SizeSquared2D());
						auto NewRotation = VelocityList[EntityIndex].Value.ToOrientationQuat();
						CurrentTransform.SetRotation(FMath::QInterpTo(CurrentTransform.GetRotation(), NewRotation, DeltaTime, 12.0f));
					}
					break;
				}
				case EFPSimpleEnemyState::Idle:
				case EFPSimpleEnemyState::Attacking: // rotate to target data
				{
					auto& TargetData = TargetDataList[EntityIndex].TargetData;
					FVector TargetLocation;
					if (UFPGAGameplayAbilitiesLibrary::GetLocationFromTargetData(TargetData, 0, TargetLocation))
					{
						FRotator CurrRotation = CurrentTransform.Rotator();
						FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(CurrentTransform.GetLocation(), TargetLocation);
						CurrentTransform.SetRotation(FMath::RInterpTo(CurrRotation, NewRotation, DeltaTime, 12.0f).Quaternion());
					}
					break;
				}
				default: ;
			}
		}
	});
}
