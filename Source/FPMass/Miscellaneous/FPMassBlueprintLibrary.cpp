// Fill out your copyright notice in the Description page of Project Settings.

#include "FPMassBlueprintLibrary.h"

#include "FPAbilitySystemFragments.h"
#include "Common/Misc/MSBPFunctionLibrary.h"
#include "FPMass/ISMRepresentation/FPISMRepresentationFragments.h"

void UFPMassBlueprintLibrary::PlayISMAnimation(FMSEntityViewBPWrapper EntityHandle, int AnimIndex, FFPISMAnimationCallbacks AnimationCallbacks)
{
	const FFPISMParameters& ISMParameters = EntityHandle.EntityView.GetConstSharedFragmentData<FFPISMParameters>();
	if (FFPISMAnimationFragment* AnimState = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMAnimationFragment>())
	{
		if (ISMParameters.AnimToTextureData)
		{
			if (ISMParameters.AnimToTextureData->Animations.IsValidIndex(AnimIndex))
			{
				FAnimToTextureAnimInfo& AnimInfo = ISMParameters.AnimToTextureData->Animations[AnimIndex];

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

UAbilitySystemComponent* UFPMassBlueprintLibrary::GetAbilitySystemFromEntity(const FMSEntityViewBPWrapper EntityHandle)
{
	if (FFPAbilitySystemFragment* MassFragmentPtr = EntityHandle.EntityView.GetFragmentDataPtr<FFPAbilitySystemFragment>())
	{
		return MassFragmentPtr->AbilitySystem.Get();
	}

	return nullptr;
}
