// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSimpleEnemyProcessor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AnimToTextureDataAsset.h"
#include "FPAIFragments.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "MassRepresentationTypes.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "FPMass/ISMRepresentation/FPISMRepresentationTrait.h"
#include "FPMass/Miscellaneous/FPAbilitySystemFragments.h"

UFPSimpleEnemyProcessor::UFPSimpleEnemyProcessor() : EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Tasks);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Avoidance);
	bRequiresGameThreadExecution = true; // maybe we don't need this?
}

void UFPSimpleEnemyProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);

	EntityQuery.AddRequirement<FFPAbilitySystemFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FFPTargetDataFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FFPISMAnimationFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FFPSimpleEnemyStateFragment>(EMassFragmentAccess::ReadWrite);

	EntityQuery.AddConstSharedRequirement<FFPISMParameters>();
	EntityQuery.AddConstSharedRequirement<FFPSimpleEnemyParameters>();
}

void UFPSimpleEnemyProcessor::Initialize(UObject& Owner)
{
	NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
}

void UFPSimpleEnemyProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	// Query mass for transform data
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [&,this](FMassExecutionContext& Context)
	{
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UFPSimpleEnemyProcessor::Execute"), STAT_FPSimpleEnemyProcessor_Execute, STATGROUP_Game);
		const int32 QueryLength = Context.GetNumEntities();

		const TArrayView<FMassVelocityFragment> Velocities = Context.GetMutableFragmentView<FMassVelocityFragment>();
		const TArrayView<FMassMoveTargetFragment> MoveToTargetFragments = Context.GetMutableFragmentView<FMassMoveTargetFragment>();

		const auto& AbilitySystemList = Context.GetFragmentView<FFPAbilitySystemFragment>();
		const auto& TargetDataList = Context.GetMutableFragmentView<FFPTargetDataFragment>();
		const auto& EnemyStateList = Context.GetMutableFragmentView<FFPSimpleEnemyStateFragment>();
		const auto& AnimStateList = Context.GetMutableFragmentView<FFPISMAnimationFragment>();

		const auto& EnemyParameters = Context.GetConstSharedFragment<FFPSimpleEnemyParameters>();
		const auto& ISMParameters = Context.GetConstSharedFragment<FFPISMParameters>();

		const TConstArrayView<FTransformFragment> Transforms = Context.GetFragmentView<FTransformFragment>();

		for (int32 i = 0; i < QueryLength; ++i)
		{
			const FTransform& Transform = Transforms[i].GetTransform();
			auto& EnemyState = EnemyStateList[i];

			// this is needed because there are multiple chunks for each shared niagara system
			FMassVelocityFragment& Velocity = Velocities[i];
			FMassMoveTargetFragment& MoveToTarget = MoveToTargetFragments[i];
			auto& AbilitySystem = AbilitySystemList[i].AbilitySystem;
			FFPTargetDataFragment& TargetData = TargetDataList[i];

			auto& AnimState = AnimStateList[i];

			FVector TargetLocation = MoveToTarget.Center;

			FVector ToTarget = MoveToTarget.Center - Transform.GetLocation();

			AActor* TargetActor = nullptr;

			// read target data
			if (auto TargetDataActor = UFPGAGameplayAbilitiesLibrary::GetFirstActorFromTargetData(TargetData.TargetData))
			{
				TargetActor = TargetDataActor;
			}
			else // check if player is close and set target data if applicable
			{
				if (APlayerController* PC = Context.GetWorld()->GetFirstPlayerController())
				{
					APawn* PlayerPawn = PC->GetPawnOrSpectator();
					if (PlayerPawn)
					{
						if ((PlayerPawn->GetActorLocation() - Transform.GetLocation()).SizeSquared2D() <= 5000 * 5000)
						{
							TargetData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(PlayerPawn);
							TargetActor = PlayerPawn;
						}
					}
				}
			}

			if (TargetActor)
			{
				TargetLocation = TargetActor->GetActorLocation();
			}
			else
			{
				UFPGAGameplayAbilitiesLibrary::GetLocationFromTargetData(TargetData.TargetData, 0, TargetLocation);
			}

			// DrawDebugLine(GetWorld(), Transform.GetLocation(), MoveToTarget.Center, FColor::Red, false, 0.0f, 0, 3.0f);
			// DrawDebugPoint(GetWorld(), MoveToTarget.Center, 24.0f, FColor::Red, false, 0.0f);

			// we reached the target point

			switch (EnemyState.State)
			{
				case EFPSimpleEnemyState::Idle:
				{
					// wait 3 sec before deciding a new location to move to
					if (EnemyState.LastMoved >= 0.25f)
					{
						constexpr float MaxWanderDistance = 3000.0f;

						FNavLocation TargetNavLocation;
						if (NavigationSystem && NavigationSystem->GetRandomReachablePointInRadius(Transform.GetLocation(), MaxWanderDistance, TargetNavLocation))
						{
							TargetLocation = TargetNavLocation;
						}
						else
						{
							FVector2D RandomDelta = FMath::RandPointInCircle(MaxWanderDistance);
							TargetLocation = Transform.GetLocation() + FVector(RandomDelta.X, RandomDelta.Y, 0.0f);
						}

						EnemyState.LastMoved = 0.0f;
						EnemyState.State = EFPSimpleEnemyState::Moving;
						MoveToTarget.CreateNewAction(EMassMovementAction::Move, *Context.GetWorld());
						// UE_LOG(LogTemp, Warning, TEXT("START MOVING"));
					}
					else
					{
						EnemyState.LastMoved += Context.GetWorld()->GetDeltaSeconds();
						TargetLocation = Transform.GetLocation();
					}

					Velocity.Value = FVector::Zero();

					// UE_LOG(LogTemp, Warning, TEXT("IDLE: %f"), AIState.LastMoved);

					break;
				}
				case EFPSimpleEnemyState::Moving:
				{
					// when we reach the target
					if (MoveToTarget.Center.IsZero() || ToTarget.SizeSquared2D() <= 350 * 350)
					{
						if (TargetActor)
						{
							FFPISMAnimationState NewAnimation;
							// NewAnimation.NumFrames = 41;
							// NewAnimation.StartFrame = 82;
							if (ISMParameters.AnimToTextureData)
							{
								if (ISMParameters.AnimToTextureData->Animations.IsValidIndex(EnemyParameters.AttackAnimationIndex))
								{
									FAnimInfo& AnimInfo = ISMParameters.AnimToTextureData->Animations[EnemyParameters.AttackAnimationIndex];
									NewAnimation.NumFrames = AnimInfo.NumFrames;
									NewAnimation.StartFrame = AnimInfo.AnimStart;
								}
								// UE_LOG(LogTemp, Warning, TEXT("%d %d"), NewAnimation.NumFrames, NewAnimation.StartFrame);
							}

							AnimState.CurrentMontage = NewAnimation;

							EnemyState.State = EFPSimpleEnemyState::Attacking;
							Velocity.Value = FVector::Zero();

							// apply ability if we have a target
							if (IsValid(EnemyParameters.AttackAbility) && AbilitySystem.IsValid())
							{
								// UE_LOG(LogTemp, Warning, TEXT("APPLY ABILITY~"));
								// AbilitySystem->TryActivateAbilityByClass(AbilityParamsList.AttackAbility);

								if (FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromClass(EnemyParameters.AttackAbility))
								{
									FGameplayTag EventTag; // = EventData.EventTag.IsValid() ? EventData.EventTag : FFPGAGlobalTags::Get().Misc_Dummy;
									FGameplayEventData EventData;
									EventData.TargetData = TargetData.TargetData;
									if (AbilitySystem->TriggerAbilityFromGameplayEvent(Spec->Handle, AbilitySystem->AbilityActorInfo.Get(), EventTag, &EventData, *AbilitySystem))
									{
										DrawDebugString(Context.GetWorld(), Transform.GetLocation(), "ATTACK", nullptr, FColor::Red, 1.0f);
										// UE_LOG(LogTemp, Warning, TEXT("Tried to attack"));
									}
									else
									{
										UE_LOG(LogTemp, Warning, TEXT("Failed to attack"));
									}
								}
							}
						}
						else // idle
						{
							EnemyState.State = EFPSimpleEnemyState::Attacking;
							Velocity.Value = FVector::Zero();
						}

						// MoveToTarget.CreateNewAction(EMassMovementAction::Stand, *Context.GetWorld());
						// UE_LOG(LogTemp, Warning, TEXT("ATTACK!"));
					}

					// UE_LOG(LogTemp, Warning, TEXT("MOVING: %f"), AIState.LastMoved);

					break;
				}
				case EFPSimpleEnemyState::Attacking:
				{
					if (!AnimState.CurrentMontage)
					{
						EnemyState.State = EFPSimpleEnemyState::Idle;
						// MoveToTarget.CreateNewAction(EMassMovementAction::Stand, *Context.GetWorld());
						// UE_LOG(LogTemp, Warning, TEXT("STOP ANIM"));
					}

					Velocity.Value = FVector::Zero();

					// UE_LOG(LogTemp, Warning, TEXT("ATTACKING: %f"), AIState.LastMoved);

					break;
				}
				default: ;
			}

			FVector TargetVector = TargetLocation - Transform.GetLocation();
			MoveToTarget.Forward = TargetVector.GetSafeNormal2D();
			MoveToTarget.Center = TargetLocation;
			MoveToTarget.DistanceToGoal = TargetVector.Size2D();
			MoveToTarget.DesiredSpeed = FMassInt16Real(1000.0f);
			MoveToTarget.SlackRadius = 100.0f;
			MoveToTarget.IntentAtGoal = EMassMovementAction::Stand;
			// UE_LOG(LogTemp, Warning, TEXT("%d %d"), (int) MoveToTarget.GetCurrentActionID(), (int) MoveToTarget.GetCurrentAction());
			// UE_LOG(LogTemp, Warning, TEXT("%f"), MoveToTarget.DistanceToGoal);

			// DrawDebugSphere(Context.GetWorld(), TargetLocation, 50.0f, 4, FColor::Red, false, 0.0f);
		}
	});
}
