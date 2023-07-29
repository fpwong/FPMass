// Fill out your copyright notice in the Description page of Project Settings.

#include "FPDebugRepresentationProcessor.h"

#include "MassCommonFragments.h"
#include "MassCommonUtils.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassSignalSubsystem.h"

UFPDebugRepresentationProcessor::UFPDebugRepresentationProcessor() : EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::Client);
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Movement);
	bRequiresGameThreadExecution = true;
}

void UFPDebugRepresentationProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);

	EntityQuery.AddTagRequirement<FMassFPDebugRepresentationTag>(EMassFragmentPresence::All);
}

void UFPDebugRepresentationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	// Query mass for transform data
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [&,this](FMassExecutionContext& Context)
	{
		const int32 NumEntities = Context.GetNumEntities();

		const auto& TransformList = Context.GetFragmentView<FTransformFragment>();

		for (int i = 0; i < NumEntities; ++i)
		{
			const auto Location = TransformList[i].GetTransform().GetLocation();

			DrawDebugSphere(Context.GetWorld(), Location, 50, 32, FColor::Red, false, 0.0);
		}
	});
}
