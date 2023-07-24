﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "FPISMRepresentationProcessor.h"

#include "FPISMActor.h"
#include "FPISMRepresentationTrait.h"
#include "FPISMSubsystem.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassRepresentationTypes.h"
#include "MassSignalSubsystem.h"
#include "Components/InstancedStaticMeshComponent.h"

UFPISMRepresentationProcessors::UFPISMRepresentationProcessors()
{
	ExecutionFlags = (int32)(EProcessorExecutionFlags::Client | EProcessorExecutionFlags::Standalone);
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Representation);
	bRequiresGameThreadExecution = true;
}

void UFPISMRepresentationProcessors::ConfigureQueries()
{
	PositionToNiagaraFragmentQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	PositionToNiagaraFragmentQuery.AddRequirement<FFPISMAnimationFragment>(EMassFragmentAccess::ReadOnly);
	PositionToNiagaraFragmentQuery.AddRequirement<FFPISMInstanceIdFragment>(EMassFragmentAccess::ReadWrite);
	PositionToNiagaraFragmentQuery.AddConstSharedRequirement<FFPISMSharedFragment>();
	PositionToNiagaraFragmentQuery.RegisterWithProcessor(*this);
}

void UFPISMRepresentationProcessors::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	// Query mass for transform data
	PositionToNiagaraFragmentQuery.ForEachEntityChunk(EntityManager, Context, [&,this](FMassExecutionContext& Context)
	{
		// UE_LOG(LogTemp, Warning, TEXT("ISM Update"));
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UFPISMRepresentationProcessors::Execute"), STAT_FPISMRepresentationProcessors_Execute, STATGROUP_Game);
		const int32 NumEntities = Context.GetNumEntities();

		const auto& TransformList = Context.GetFragmentView<FTransformFragment>();
		const auto& AnimationList = Context.GetFragmentView<FFPISMAnimationFragment>();
		const auto& InstanceIdList = Context.GetMutableFragmentView<FFPISMInstanceIdFragment>();

		const auto& ISMSharedFragment = Context.GetConstSharedFragment<FFPISMSharedFragment>();

		if (UFPISMSubsystem* ISMSubsystem = Context.GetWorld()->GetSubsystem<UFPISMSubsystem>())
		{
			if (AFPISMActor* ISMActor = ISMSubsystem->FindOrCreateISM(ISMSharedFragment.StaticMesh.LoadSynchronous()))
			{
				for (int i = 0; i < NumEntities; ++i)
				{
					FFPISMInstanceIdFragment& InstanceId = InstanceIdList[i];

					FTransform Transform = ISMSharedFragment.RelativeTransform * TransformList[i].GetTransform();

					const FFPISMAnimationFragment& Anim = AnimationList[i];

					uint32 EntityId = GetTypeHash(Context.GetEntity(i));

					ISMActor->SharedData.UpdateInstanceIds.Add(EntityId);
					ISMActor->SharedData.StaticMeshInstanceTransforms.Add(Transform);
					ISMActor->SharedData.StaticMeshInstancePrevTransforms.Add(InstanceId.PrevTransform);

					// TArray<float> BlendData = { WalkBlend, AnimationState.MontageBlend, IdleWalkFrame, MontageCurrFrame, MontageStartFrame, MontageNumFrames };
					// const auto CustomData = { Anim.WalkBlend, Anim.MontageBlend, , 0.0f, 0.0f, 0.0f};
					auto CustomData = Anim.AsCustomData();
					// UE_LOG(LogTemp, Warning, TEXT("%f %f %f %f %f %f"), CustomData[0], CustomData[1], CustomData[2], CustomData[3], CustomData[4], CustomData[5]);
					ISMActor->SharedData.StaticMeshInstanceCustomFloats.Append(CustomData);

					InstanceId.PrevTransform = Transform;

					// DrawDebugString(Context.GetWorld(), Transform.GetLocation(), FString::FromInt(EntityId), nullptr, FColor::White, 0.0f);
				}
			}
		}
	});
}
