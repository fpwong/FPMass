// Fill out your copyright notice in the Description page of Project Settings.

#include "FPISMRepresentationTrait.h"

#include "FPISMSubsystem.h"
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

void UFPISMRepresentationTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	if (!World.IsGameWorld())
	{
		return;
	}

	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	const FConstSharedStruct& ISMParameters = EntityManager.GetOrCreateConstSharedFragment(ISMSharedParameters);
	BuildContext.AddConstSharedFragment(ISMParameters);

	BuildContext.RequireFragment<FTransformFragment>();

	BuildContext.AddFragment<FFPISMStateFragment>();
	BuildContext.AddFragment<FFPISMAnimationFragment>();

	FFPISMRepresentationFragment& RepFrag = BuildContext.AddFragment_GetRef<FFPISMRepresentationFragment>();
	RepFrag = Representation;
}
