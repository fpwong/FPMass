// Fill out your copyright notice in the Description page of Project Settings.

#include "FPNavMeshSnappingProcessor.h"

#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "AI/NavigationSystemBase.h"

UFPNavMeshSnappingProcessor::UFPNavMeshSnappingProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Movement);
	bAutoRegisterWithProcessingPhases = false;
}

void UFPNavMeshSnappingProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	// EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
}

void UFPNavMeshSnappingProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);
	// MassNavigationSubsystem = UWorld::GetSubsystem<UMassNavigationSubsystem>(Owner.GetWorld());
	NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
}

void UFPNavMeshSnappingProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	// QUICK_SCOPE_CYCLE_COUNTER(UFPNavMeshSnappingProcessor)
	if (!NavigationSystem)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Nav sys invalid"));
		return;
	}

	// Avoidance while standing
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this, &EntityManager](FMassExecutionContext& Context)
	{
		const int32 NumEntities = Context.GetNumEntities();
		TArrayView<FTransformFragment> TransformList = Context.GetMutableFragmentView<FTransformFragment>();

		for (int32 EntityIndex = 0; EntityIndex < NumEntities; ++EntityIndex)
		{
			FTransform& Transform = TransformList[EntityIndex].GetMutableTransform();
			FVector CurrentLocation = Transform.GetLocation();

			FNavLocation NewNavLocation;
			if (NavigationSystem && NavigationSystem->ProjectPointToNavigation(CurrentLocation, NewNavLocation))
			{
				FVector NewLocation = NewNavLocation.Location;
				NewLocation.Z = CurrentLocation.Z;
				Transform.SetLocation(NewLocation);
			}

			// DrawDebugPoint(Context.GetWorld(), CurrentLocation, 16, FColor::Red, false, 0.0f);
		}
	});
}