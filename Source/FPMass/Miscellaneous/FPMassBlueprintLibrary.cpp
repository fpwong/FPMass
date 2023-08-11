// Fill out your copyright notice in the Description page of Project Settings.

#include "FPMassBlueprintLibrary.h"

#include "FPAbilitySystemFragments.h"
#include "Common/Misc/MSBPFunctionLibrary.h"
#include "FPMass/ISMRepresentation/FPISMRepresentationFragments.h"

void UFPMassBlueprintLibrary::PlayISMAnimation(FMSEntityViewBPWrapper EntityHandle, int AnimIndex, FFPISMAnimationCallbacks AnimationCallbacks)
{
	// const FFPISMParameters& ISMParameters = EntityHandle.EntityView.GetConstSharedFragmentData<FFPISMParameters>();
	if (const FFPISMRepresentationFragment* Representation = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMRepresentationFragment>())
	{
		if (FFPISMAnimationFragment* AnimState = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMAnimationFragment>())
		{
			if (Representation->ISMDescription.AnimToTextureData)
			{
				if (Representation->ISMDescription.AnimToTextureData->Animations.IsValidIndex(AnimIndex))
				{
					FAnimToTextureAnimInfo& AnimInfo = Representation->ISMDescription.AnimToTextureData->Animations[AnimIndex];

					FFPISMAnimationState NewAnimation;
					NewAnimation.AnimIndex = AnimIndex;
					NewAnimation.NumFrames = AnimInfo.EndFrame - AnimInfo.StartFrame;
					NewAnimation.StartFrame = AnimInfo.StartFrame;
					NewAnimation.AnimationCallbacks = AnimationCallbacks;

					AnimState->CurrentMontage = NewAnimation;
				}
			}
		}
	}
}

UAbilitySystemComponent* UFPMassBlueprintLibrary::GetAbilitySystemFromEntity(const FMSEntityViewBPWrapper EntityHandle)
{
	if (FFPAbilitySystemFragment* MassFragmentPtr = EntityHandle.EntityView.GetFragmentDataPtr<FFPAbilitySystemFragment>())
	{
		return MassFragmentPtr->AbilitySystem.Get();
	}

	return nullptr;
}

bool UFPMassBlueprintLibrary::SetEntityISMRepresentation(const FMSEntityViewBPWrapper EntityHandle, FFPISMRepresentationFragment Representation)
{
	if (!EntityHandle.EntityView.GetEntity().IsValid())
	{
		return false;
	}

	if (FFPISMRepresentationFragment* RepresentationFrag = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMRepresentationFragment>())
	{
		*RepresentationFrag = Representation;
	}

	return true;
}

FFPISMRepresentationFragment UFPMassBlueprintLibrary::GetEntityISMRepresentation(const FMSEntityViewBPWrapper EntityHandle)
{
	if (EntityHandle.EntityView.GetEntity().IsValid())
	{
		if (FFPISMRepresentationFragment* RepresentationFrag = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMRepresentationFragment>())
		{
			return *RepresentationFrag;
		}
	}

	return FFPISMRepresentationFragment();
}
