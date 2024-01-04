// Fill out your copyright notice in the Description page of Project Settings.

#include "FPISMRepresentationTrait.h"

#include "FPISMSubsystem.h"
#include "GameplayTagContainer.h"
#include "MassCommonFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntityView.h"

TArray<float> FFPISMAnimationFragment::AsCustomData() const
{
	float IdleWalkFrame = 0;
	float MontageCurrFrame = 0;
	float MontageStartFrame = 0;
	float MontageNumFrames = 0;

	if (CurrentMontage.IsSet())
	{
		MontageCurrFrame = CurrentMontage->CurrentFrame;
		MontageStartFrame = CurrentMontage->StartFrame;
		MontageNumFrames = CurrentMontage->NumFrames;
	}

	return { WalkBlend, MontageBlend, IdleWalkFrame, MontageCurrFrame, MontageStartFrame, MontageNumFrames };
}

FGuid FFPISMRepresentationFragment::AddLayer(const FGameplayTag& LayerTag, const FFPISMDescription& Description)
{
	FFPISMDescriptionLayer NewLayer;
	NewLayer.Description = Description;
	NewLayer.Guid = FGuid::NewGuid();
	Layers.FindOrAdd(LayerTag).Layers.Add(NewLayer);
	return NewLayer.Guid;
}

void UFPISMRepresentationTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	if (!World.IsGameWorld())
	{
		return;
	}

	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	// const FConstSharedStruct& ISMParameters = EntityManager.GetOrCreateConstSharedFragment(ISMSharedParameters);
	// BuildContext.AddConstSharedFragment(ISMParameters);

	BuildContext.RequireFragment<FTransformFragment>();

	BuildContext.AddFragment<FFPISMStateFragment>();
	BuildContext.AddFragment<FFPISMAnimationFragment>();

	FFPISMRepresentationFragment& RepFrag = BuildContext.AddFragment_GetRef<FFPISMRepresentationFragment>();
	// RepFrag = Representation;
	RepFrag.RelativeTransform = RelativeTransform;
	RepFrag.AddLayer(FGameplayTag::RequestGameplayTag("ISMLayer.Main"), ISMDescription);// = RelativeTransform;
}

// #if WITH_EDITORONLY_DATA
// void UFPISMRepresentationTrait::Serialize(FArchive& Ar)
// {
// 	Super::Serialize(Ar);
//
// 	if (ISMDescription.AnimToTextureData == nullptr)
// 	{
// 		ISMDescription = Representation.ISMDescription;
// 		MarkPackageDirty();
// 	}
// }
// #endif