// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSimpleWanderProcessor.h"
#include "MassCommonFragments.h"
#include "MassEntitySubsystem.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"

UFPSimpleWanderProcessor::UFPSimpleWanderProcessor() : EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = false;
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Tasks);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Avoidance);
	bRequiresGameThreadExecution = true; // maybe we don't need this?
}

void UFPSimpleWanderProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddTagRequirement<FFPMassTag_SimpleWander>(EMassFragmentPresence::All);
}

void UFPSimpleWanderProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);
	NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
}

void UFPSimpleWanderProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	// Query mass for transform data
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [&,this](FMassExecutionContext& Context)
	{
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UFPSimpleWanderProcessor::Execute"), STAT_FPSimpleWanderProcessor_Execute, STATGROUP_Game);
		const int32 QueryLength = Context.GetNumEntities();

		const TArrayView<FMassVelocityFragment> Velocities = Context.GetMutableFragmentView<FMassVelocityFragment>();
		const TArrayView<FMassMoveTargetFragment> MoveToTargetFragments = Context.GetMutableFragmentView<FMassMoveTargetFragment>();
		const TConstArrayView<FTransformFragment> Transforms = Context.GetFragmentView<FTransformFragment>();

		for (int32 i = 0; i < QueryLength; ++i)
		{
			const FTransform& Transform = Transforms[i].GetTransform();

			// this is needed because there are multiple chunks for each shared niagara system
			FMassVelocityFragment& Velocity = Velocities[i];
			FMassMoveTargetFragment& MoveToTarget = MoveToTargetFragments[i];

			FVector TargetLocation = MoveToTarget.Center;
			FVector ToTarget = MoveToTarget.Center - Transform.GetLocation();

			// read target data // wait 3 sec before deciding a new location to move to
			if (ToTarget.SquaredLength() <= 150 * 150)
			{
				constexpr float MaxWanderDistance = 1000.0f;

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

				MoveToTarget.CreateNewAction(EMassMovementAction::Move, *Context.GetWorld());
			}

			FVector TargetVector = TargetLocation - Transform.GetLocation();
			MoveToTarget.Forward = TargetVector.GetSafeNormal2D();
			MoveToTarget.Center = TargetLocation;
			MoveToTarget.DistanceToGoal = TargetVector.Size2D();
			MoveToTarget.DesiredSpeed = FMassInt16Real(1000.0f);
			MoveToTarget.SlackRadius = 100.0f;
			MoveToTarget.IntentAtGoal = EMassMovementAction::Stand;
		}
	});
}