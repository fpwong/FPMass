﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSimpleOrientation.h"

#include "AbilitySystemComponent.h"
#include "FPSimpleEnemyProcessor.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "AbilitySystem/FPGATypes.h"
#include "FPMass/AI/FPAIFragments.h"
#include "FPMass/Miscellaneous/FPAbilitySystemFragments.h"
#include "Kismet/KismetMathLibrary.h"

class UFPGameplayInterface;

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
	EntityQuery.AddRequirement<FFPAbilitySystemFragment>(EMassFragmentAccess::ReadOnly);
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
		const auto& AbilitySystemList = Context.GetFragmentView<FFPAbilitySystemFragment>();

		for (int32 EntityIndex = 0; EntityIndex < NumEntities; ++EntityIndex)
		{
			FTransform& CurrentTransform = LocationList[EntityIndex].GetMutableTransform();
			auto& EnemyState = EnemyStateList[EntityIndex];
			auto& AbilitySystem = AbilitySystemList[EntityIndex].AbilitySystem;

			// skip if we are unable to rotate
			if (AbilitySystem.IsValid() && AbilitySystem->Implements<UFPGameplayInterface>())
			{
				if (IFPGameplayInterface* Interface = Cast<IFPGameplayInterface>(AbilitySystem.Get()))
				{
					if (!Interface->CanRotate(AbilitySystem.Get()))
					{
						continue;
					}
				}
			}

			float InterpSpeed = 6.0f;

			bool bTryLookAtTargetData = false;
			TOptional<FVector> LookAtTarget;

			switch (EnemyState.State)
			{
				case EFPSimpleEnemyState::Moving: // rotate to velocity
				{
					// Snap position to move target directly
					FVector Velocity = VelocityList[EntityIndex].Value;
					float SpeedSquared = Velocity.SizeSquared2D();
					if (SpeedSquared >= 300 * 300)
					{
						// UE_LOG(LogTemp, Warning, TEXT("Velocity %f"), Velocity.SizeSquared2D());
						LookAtTarget = CurrentTransform.GetLocation() + VelocityList[EntityIndex].Value;

						// float Speed = FMath::Sqrt(SpeedSquared);
						// InterpSpeed = FMath::Clamp(Speed / 50.0f, 3.0f, 12.0f);
					}
					else
					{
						bTryLookAtTargetData = true;
					}
					break;
				}
				case EFPSimpleEnemyState::Idle:
				case EFPSimpleEnemyState::Attacking: // rotate to target data
				{
					bTryLookAtTargetData = true;
					InterpSpeed = 10.0f;
					break;
				}
				default: ;
			}

			const float FinalRotationSpeed = InterpSpeed * EnemyState.RotationSpeed * EnemyState.ActionSpeed;
			if (FinalRotationSpeed <= 0.0f)
			{
				continue;
			}

			if (bTryLookAtTargetData)
			{
				FGameplayAbilityTargetDataHandle& TargetData = TargetDataList[EntityIndex].TargetData;
				FVector TargetLocation;
				if (UFPGAGameplayAbilitiesLibrary::GetLocationFromTargetData(TargetData, 0, TargetLocation))
				{
					LookAtTarget = TargetLocation;
				}
			}

			if (LookAtTarget.IsSet())
			{
				FRotator CurrRotation = CurrentTransform.Rotator();
				FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(CurrentTransform.GetLocation(), LookAtTarget.GetValue());
				NewRotation.Pitch = 0;
				NewRotation.Roll = 0;
				CurrentTransform.SetRotation(FMath::RInterpTo(CurrRotation, NewRotation, DeltaTime, FinalRotationSpeed).Quaternion());
			}
		}
	});
}
