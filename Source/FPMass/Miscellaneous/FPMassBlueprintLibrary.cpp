// Fill out your copyright notice in the Description page of Project Settings.

#include "FPMassBlueprintLibrary.h"

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
				FAnimInfo& AnimInfo = ISMParameters.AnimToTextureData->Animations[AnimIndex];

				FFPISMAnimationState NewAnimation;
				NewAnimation.AnimIndex = AnimIndex;
				NewAnimation.NumFrames = AnimInfo.NumFrames;
				NewAnimation.StartFrame = AnimInfo.AnimStart;
				NewAnimation.AnimationCallbacks = AnimationCallbacks;

				AnimState->CurrentMontage = NewAnimation;
			}
		}
	}
}
