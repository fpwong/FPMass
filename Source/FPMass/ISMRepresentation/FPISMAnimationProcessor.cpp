// Fill out your copyright notice in the Description page of Project Settings.

#include "FPISMAnimationProcessor.h"

#include "FPISMActor.h"
#include "FPISMRepresentationTrait.h"
#include "FPISMSubsystem.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassRepresentationTypes.h"
#include "MassSignalSubsystem.h"

UFPISMAnimationProcessors::UFPISMAnimationProcessors() : EntityQuery(*this)
{
	ExecutionFlags = (int32)(EProcessorExecutionFlags::Client | EProcessorExecutionFlags::Standalone);
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Movement);
	bRequiresGameThreadExecution = false;
}

void UFPISMAnimationProcessors::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);

	EntityQuery.AddRequirement<FFPISMAnimationFragment>(EMassFragmentAccess::ReadWrite);
}

void UFPISMAnimationProcessors::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	// Query mass for transform data
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [&,this](FMassExecutionContext& Context)
	{
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UFPISMAnimationProcessors::Execute"), STAT_FPISMAnimationProcessors_Execute, STATGROUP_Game);
		const int32 NumEntities = Context.GetNumEntities();

		// const auto& TransformList = Context.GetFragmentView<FTransformFragment>();
		const auto& VelocityList = Context.GetFragmentView<FMassVelocityFragment>();
		const auto& AnimationList = Context.GetMutableFragmentView<FFPISMAnimationFragment>();

		float DeltaTime = Context.GetWorld()->DeltaTimeSeconds;

		for (int i = 0; i < NumEntities; ++i)
		{
			// const auto& Transform = TransformList[i].GetTransform();
			const auto& Velocity = VelocityList[i].Value;
			FFPISMAnimationFragment& AnimationState = AnimationList[i];

			float MaxSpeed = 400.0f;
			float TargetWalkBlend = FMath::Clamp(Velocity.SizeSquared2D() / (MaxSpeed * MaxSpeed), 0.0, 1.0);
			AnimationState.WalkBlend = FMath::FInterpTo(AnimationState.WalkBlend, TargetWalkBlend, DeltaTime, 8.0f);

			float TargetMontageBlend = 0;

			if (AnimationState.CurrentMontage.IsSet())
			{
				FFPISMAnimationState& Animation = AnimationState.CurrentMontage.GetValue();

				TargetMontageBlend = 1;

				// update current frame
				float Delta = Context.GetWorld()->GetDeltaSeconds() * 30.0f; // 30 fps?
				Animation.CurrentFrame += Delta;

				// stop playing the animation if we exceed the number of frames
				if (Animation.CurrentFrame >= Animation.NumFrames)
				{
					AnimationState.CurrentMontage.Reset();
					// TODO how do signals work?
				}
			}

			AnimationState.MontageBlend = FMath::FInterpTo(AnimationState.MontageBlend, TargetMontageBlend, DeltaTime, 8.0f);
		}
	});
}
