// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSimpleVelocityOrientationProcessor.h"

#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "Kismet/KismetMathLibrary.h"

UFPSimpleVelocityOrientationProcessor::UFPSimpleVelocityOrientationProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Movement;
}

void UFPSimpleVelocityOrientationProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddTagRequirement<FFPMassTag_VelocityOrientation>(EMassFragmentPresence::All);
}

void UFPSimpleVelocityOrientationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	// Clamp max delta time to avoid force explosion on large time steps (i.e. during initialization).
	const float DeltaTime = FMath::Min(0.1f, Context.GetDeltaTimeSeconds());

	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this, DeltaTime](FMassExecutionContext& Context)
	{
		const int32 NumEntities = Context.GetNumEntities();

		const auto& LocationList = Context.GetMutableFragmentView<FTransformFragment>();
		const auto& VelocityList = Context.GetMutableFragmentView<FMassVelocityFragment>();

		for (int32 EntityIndex = 0; EntityIndex < NumEntities; ++EntityIndex)
		{
			FTransform& CurrentTransform = LocationList[EntityIndex].GetMutableTransform();

			FVector LookAtTarget = CurrentTransform.GetLocation() + VelocityList[EntityIndex].Value;
			FRotator CurrRotation = CurrentTransform.Rotator();
			FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(CurrentTransform.GetLocation(), LookAtTarget);

			NewRotation.Pitch = 0;
			NewRotation.Roll = 0;
			CurrentTransform.SetRotation(FMath::RInterpTo(CurrRotation, NewRotation, DeltaTime, 12.0f).Quaternion());
		}
	});
}