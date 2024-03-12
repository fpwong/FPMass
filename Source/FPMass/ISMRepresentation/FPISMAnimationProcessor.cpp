// Fill out your copyright notice in the Description page of Project Settings.

#include "FPISMAnimationProcessor.h"

#include "FPISMRepresentationTrait.h"
#include "FPISMSubsystem.h"
#include "MassCommonFragments.h"
#include "MassCommonUtils.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassSignalSubsystem.h"
#include "FPMass/AI/FPSimpleEnemyProcessor.h"
#include "FPVertexAnimationTextures/FPAnimToTextureDataAsset.h"

#define USE_GAME_THREAD false

UFPISMAnimationProcessors::UFPISMAnimationProcessors() : EntityQuery(*this)
{
	ExecutionFlags = (int32)(EProcessorExecutionFlags::Client | EProcessorExecutionFlags::Standalone);
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Movement);
	bRequiresGameThreadExecution = USE_GAME_THREAD;
}

void UFPISMAnimationProcessors::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FFPISMRepresentationFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FFPSimpleEnemyStateFragment>(EMassFragmentAccess::ReadOnly);

	EntityQuery.AddRequirement<FFPISMAnimationFragment>(EMassFragmentAccess::ReadWrite);

	// EntityQuery.AddConstSharedRequirement<FFPISMParameters>();
}

void UFPISMAnimationProcessors::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	// Query mass for transform data
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [&,this](FMassExecutionContext& Context)
	{
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UFPISMAnimationProcessors::Execute"), STAT_FPISMAnimationProcessors_Execute, STATGROUP_Game);
		const int32 NumEntities = Context.GetNumEntities();

		const auto& VelocityList = Context.GetFragmentView<FMassVelocityFragment>();
		const auto& RepresentationList = Context.GetFragmentView<FFPISMRepresentationFragment>();
		const auto& AnimationList = Context.GetMutableFragmentView<FFPISMAnimationFragment>();
		const auto& EnemyStateList = Context.GetMutableFragmentView<FFPSimpleEnemyStateFragment>();

		// const auto& ISMParameters = Context.GetConstSharedFragment<FFPISMParameters>();

		const float DeltaTime = Context.GetWorld()->DeltaTimeSeconds;

		// UFPAnimToTextureDataAsset* FPAnimToData = Cast<UFPAnimToTextureDataAsset>(ISMParameters.AnimToTextureData);

		for (int i = 0; i < NumEntities; ++i)
		{
			// const auto& Transform = TransformList[i].GetTransform();
			const auto& Velocity = VelocityList[i].Value;
			FFPISMAnimationFragment& AnimationState = AnimationList[i];
			const auto& Representation = RepresentationList[i];
			const auto& EnemyState = EnemyStateList[i];

			float MaxSpeed = 400.0f;
			float TargetWalkBlend = FMath::Clamp(Velocity.SizeSquared2D() / (MaxSpeed * MaxSpeed), 0.0, 1.0);
			AnimationState.WalkBlend = FMath::FInterpTo(AnimationState.WalkBlend, TargetWalkBlend, DeltaTime, 8.0f);

			float TargetMontageBlend = 0;
			float MontageInterpSpeed = 10.0f;

			if (AnimationState.CurrentMontage.IsSet())
			{
				FFPISMAnimationState& Animation = AnimationState.CurrentMontage.GetValue();

				TargetMontageBlend = 1;

				// if (EnemyState.ActionSpeed <= 0.0f)
				// {
				// 	TargetMontageBlend = 0.0f;
				// }

				// update current frame
				const float AnimSpeed = Animation.AnimSpeed * EnemyState.ActionSpeed;
				const float Delta = Context.GetWorld()->GetDeltaSeconds() * 30.0f * AnimSpeed; // 30 fps?
				MontageInterpSpeed += FMath::Max(1.0f, AnimSpeed);

				float NewFrame = Animation.CurrentFrame + Delta;

				Representation.ForEachActiveISMDescription([&](const FFPISMDescription& ISMDesc)
				{
					if (auto FPAnimToData = Cast<UFPAnimToTextureDataAsset>(ISMDesc.AnimToTextureData))
					{
						const uint8& AnimIndex = Animation.AnimIndex;
						if (FPAnimToData->AnimNotifyInfo.IsValidIndex(AnimIndex))
						{
							for (const FFPAnimNotifyEvent& NotifyEvent : FPAnimToData->AnimNotifyInfo[AnimIndex].AnimNotifies)
							{
								if (Animation.CurrentFrame <= NotifyEvent.Frame && NewFrame >= NotifyEvent.Frame)
								{
	#if USE_GAME_THREAD
									Animation.AnimationCallbacks.OnAnimNotify.ExecuteIfBound();
	#else
									Context.Defer().PushCommand<FMassDeferredSetCommand>([Value = "Foo", Callback = Animation.AnimationCallbacks.OnAnimNotify](FMassEntityManager& Manager)
									{
										Callback.ExecuteIfBound(Value);
									});
	#endif
								}
							}
						}
					}
				});

				Animation.CurrentFrame = NewFrame;

				// stop playing the animation if we exceed the number of frames
				if (Animation.CurrentFrame >= Animation.NumFrames)
				{
#if USE_GAME_THREAD
					Animation.AnimationCallbacks.OnAnimEnded.ExecuteIfBound();
#else
					Context.Defer().PushCommand<FMassDeferredSetCommand>([Callback = Animation.AnimationCallbacks.OnAnimEnded](FMassEntityManager& Manager)
					{
						Callback.ExecuteIfBound();
					});
#endif

					AnimationState.CurrentMontage.Reset();
				}
			}

			AnimationState.MontageBlend = FMath::FInterpTo(AnimationState.MontageBlend, TargetMontageBlend, DeltaTime, MontageInterpSpeed);
		}
	});
}
