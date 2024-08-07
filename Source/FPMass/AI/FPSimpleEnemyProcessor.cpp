﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSimpleEnemyProcessor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FPAIFragments.h"
#include "FPMassSettings.h"
#include "MassCommonFragments.h"
#include "MassEntitySubsystem.h"
#include "MassEntityView.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "AbilitySystem/FPGATypes.h"
#include "FPMass/ISMRepresentation/FPISMRepresentationTrait.h"
#include "FPMass/Miscellaneous/FPAbilitySystemFragments.h"

UFPSimpleEnemyProcessor::UFPSimpleEnemyProcessor() : EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Movement);
	// ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Avoidance);
	bRequiresGameThreadExecution = true; // maybe we don't need this?
}

void UFPSimpleEnemyProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);

	EntityQuery.AddRequirement<FFPAbilitySystemFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FFPTargetDataFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FFPSimpleEnemyStateFragment>(EMassFragmentAccess::ReadWrite);

	EntityQuery.AddRequirement<FFPSimpleEnemyParameters>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FAgentRadiusFragment>(EMassFragmentAccess::ReadOnly);
}

void UFPSimpleEnemyProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);
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
		const auto& AgentRadiusList = Context.GetMutableFragmentView<FAgentRadiusFragment>();

		const auto& EnemyParametersList = Context.GetFragmentView<FFPSimpleEnemyParameters>();

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
			auto& EnemyParameters = EnemyParametersList[i];
			auto& AgentRadius = AgentRadiusList[i];

			FVector TargetLocation = MoveToTarget.Center;

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
						// ENABLE this check if you want to make the mobs have an aggro range
						// if ((PlayerPawn->GetActorLocation() - Transform.GetLocation()).SizeSquared2D() <= 5000 * 5000)
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

			FVector ToTarget = TargetLocation - Transform.GetLocation();

			switch (EnemyState.State)
			{
				case EFPSimpleEnemyState::Idle:
				{
					// wait 3 sec before deciding a new location to move to
					if (EnemyState.LastMoved >= 0.5f)
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
					}
					else
					{
						EnemyState.LastMoved += Context.GetWorld()->GetDeltaSeconds();
						TargetLocation = Transform.GetLocation();
					}

					Velocity.Value = FVector::Zero();
					break;
				}
				case EFPSimpleEnemyState::Moving:
				{
					// when we reach the target
					float AttackRange = 50.f; // TODO read from ability system

					// get attack range from attack ability CDO
					if (IsValid(EnemyParameters.AttackAbility))
					{
						if (UGameplayAbility* AbilityCDO = EnemyParameters.AttackAbility->GetDefaultObject<UGameplayAbility>())
						{
							if (AbilityCDO->Implements<UFPAbilityInterface>())
							{
								AttackRange = IFPAbilityInterface::Execute_GetAbilityRange(AbilityCDO, AbilitySystem.Get());
								AttackRange = FMath::Max(AttackRange, 50.0f);
								// UE_LOG(LogTemp, Warning, TEXT("Get ability range %f"), AttackRange);
							}
						}
					}

					AttackRange += AgentRadius.Radius;

					// if (AbilitySystem.IsValid() && UFPMassSettings::Get().AttackRangeAttribute.IsValid())
					// {
					// 	AttackRange = AbilitySystem->GetNumericAttribute(UFPMassSettings::Get().AttackRangeAttribute);
					// }

					if (MoveToTarget.Center.IsZero() || ToTarget.SizeSquared2D() <= AttackRange * AttackRange)
					{
						if (TargetActor)
						{
							// run ability if we have a target
							if (IsValid(EnemyParameters.AttackAbility) && AbilitySystem.IsValid())
							{
								// DrawDebugSphere(Context.GetWorld(), Transform.GetLocation(), 100.0f, 32, FColor::Red, true, 2.0f);
								// UE_LOG(LogTemp, Warning, TEXT("%s %s"), *Transform.GetLocation().ToCompactString(), *MoveToTarget.Center.ToCompactString());

								FGameplayEventData EventData;
								EventData.TargetData = TargetData.TargetData;

								FMassEntityHandle Entity = Context.GetEntity(i);
								FOnGameplayAbilityEnded::FDelegate OnAbilityEnded = FOnGameplayAbilityEnded::FDelegate::CreateLambda([EntityHandle = Entity](UGameplayAbility* Ability)
								{
									// UE_LOG(LogTemp, Warning, TEXT("Ability ended?"));
									if (auto World = Ability->GetWorld())
									{
										if (UMassEntitySubsystem* Subsystem = World->GetSubsystem<UMassEntitySubsystem>())
										{
											if (EntityHandle.IsValid())
											{
												const FMassEntityManager& EntityManager = Subsystem->GetEntityManager();
												auto Archetype = EntityManager.GetArchetypeForEntity(EntityHandle);
												if (Archetype.IsValid())
												{
													FMassEntityView EntityView(EntityManager, EntityHandle);

													if (FFPSimpleEnemyStateFragment* LocalEnemyState = EntityView.GetFragmentDataPtr<FFPSimpleEnemyStateFragment>())
													{
														// UE_LOG(LogTemp, Warning, TEXT("Finished gameplay ability!"));
														LocalEnemyState->State = EFPSimpleEnemyState::Idle;
													}
												}
											}
										}
									}
								});

								if (UFPGAGameplayAbilitiesLibrary::ActivateAbilityWithEvent(AbilitySystem.Get(), EnemyParameters.AttackAbility, EventData, &OnAbilityEnded))
								{
									EnemyState.State = EFPSimpleEnemyState::Attacking;
									Velocity.Value = FVector::Zero();
								}
							}
						}
						else // idle
						{
							EnemyState.State = EFPSimpleEnemyState::Idle;
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
					Velocity.Value = FVector::Zero();
					TargetLocation = Transform.GetLocation();
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

#if 0 // debug draw state
			FString CurrentState;
			switch (EnemyState.State)
			{
				case EFPSimpleEnemyState::Idle:
					CurrentState = "IDLE";
					break;
				case EFPSimpleEnemyState::Moving:
					CurrentState = "MOVING";
					break;
				case EFPSimpleEnemyState::Attacking:
					CurrentState = "ATTACKING";
					break;
				default: ;
			}

			DrawDebugString(Context.GetWorld(), Transform.GetLocation() + FVector(0, 0, 100), CurrentState, nullptr, FColor::Red, 0.0f);
#endif
		}
	});
}