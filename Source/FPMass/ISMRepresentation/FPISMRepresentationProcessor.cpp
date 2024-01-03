// Fill out your copyright notice in the Description page of Project Settings.

#include "FPISMRepresentationProcessor.h"

#include "AnimToTextureDataAsset.h"
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
	PositionToNiagaraFragmentQuery.AddRequirement<FFPISMRepresentationFragment>(EMassFragmentAccess::ReadOnly);
	PositionToNiagaraFragmentQuery.AddRequirement<FFPISMStateFragment>(EMassFragmentAccess::ReadWrite);
	// PositionToNiagaraFragmentQuery.AddConstSharedRequirement<FFPISMParameters>();
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
		const auto& RepresentationList = Context.GetFragmentView<FFPISMRepresentationFragment>();
		const auto& InstanceIdList = Context.GetMutableFragmentView<FFPISMStateFragment>();

		// const auto& ISMSharedFragment = Context.GetConstSharedFragment<FFPISMParameters>();

		if (UFPISMSubsystem* ISMSubsystem = Context.GetWorld()->GetSubsystem<UFPISMSubsystem>())
		{
			// if (AFPISMActor* ISMActor = ISMSubsystem->FindOrCreateISM(ISMSharedFragment.ISMDescription))
			{
				for (int i = 0; i < NumEntities; ++i)
				{
					const auto& Representation = RepresentationList[i];
					const FFPISMAnimationFragment& Anim = AnimationList[i];
					TArray<float> CustomData = Anim.AsCustomData();

					for (const auto& ISMDesc : Representation.ISMDescriptions)
					{
						if (AFPISMActor* ISMActor = ISMSubsystem->FindOrCreateISM(ISMDesc))
						{
							FFPISMStateFragment& InstanceId = InstanceIdList[i];

							FTransform Transform = Representation.RelativeTransform * ISMDesc.RelativeTransform * TransformList[i].GetTransform();
							// FTransform Transform = ISMSharedFragment.RelativeTransform * TransformList[i].GetTransform();

							uint32 EntityId = GetTypeHash(Context.GetEntity(i));

							ISMActor->SharedData.UpdateInstanceIds.Add(EntityId);
							ISMActor->SharedData.StaticMeshInstanceTransforms.Add(Transform);
							ISMActor->SharedData.StaticMeshInstancePrevTransforms.Add(InstanceId.PrevTransform);

							// UE_LOG(LogTemp, Warning, TEXT("%f %f %f %f %f %f"), CustomData[0], CustomData[1], CustomData[2], CustomData[3], CustomData[4], CustomData[5]);
							ISMActor->SharedData.StaticMeshInstanceCustomFloats.Append(CustomData);

							InstanceId.PrevTransform = Transform;
						}
					}
				}
			}
		}
	});
}
