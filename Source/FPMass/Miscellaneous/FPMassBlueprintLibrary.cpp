// Fill out your copyright notice in the Description page of Project Settings.

#include "FPMassBlueprintLibrary.h"

#include "FPAbilitySystemFragments.h"
#include "FPSlideMovementProcessor.h"
#include "Common/Misc/MSBPFunctionLibrary.h"
#include "FPMass/ISMRepresentation/FPISMRepresentationFragments.h"

void UFPMassBlueprintLibrary::PlayISMAnimation(FMSEntityViewBPWrapper EntityHandle, int AnimIndex, FFPISMAnimationCallbacks AnimationCallbacks)
{
	// const FFPISMParameters& ISMParameters = EntityHandle.EntityView.GetConstSharedFragmentData<FFPISMParameters>();
	if (const FFPISMRepresentationFragment* Representation = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMRepresentationFragment>())
	{
		if (FFPISMAnimationFragment* AnimState = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMAnimationFragment>())
		{
			for (const auto& ISMDesc : Representation->ISMDescriptions)
			{
				if (ISMDesc.AnimToTextureData)
				{
					if (ISMDesc.AnimToTextureData->Animations.IsValidIndex(AnimIndex))
					{
						FAnimToTextureAnimInfo& AnimInfo = ISMDesc.AnimToTextureData->Animations[AnimIndex];

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

FGuid UFPMassBlueprintLibrary::AddEntityISMDescription(const FMSEntityViewBPWrapper EntityHandle, FFPISMDescription Description)
{
	if (!EntityHandle.EntityView.GetEntity().IsValid())
	{
		return FGuid();
	}

	if (FFPISMRepresentationFragment* RepresentationFrag = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMRepresentationFragment>())
	{
		RepresentationFrag->ISMDescriptions.Add(Description);
		return Description.Guid; 
	}

	return FGuid();
}

bool UFPMassBlueprintLibrary::RemoveEntityISMDescription(const FMSEntityViewBPWrapper EntityHandle, FGuid Guid)
{
	if (!EntityHandle.EntityView.GetEntity().IsValid())
	{
		return false;
	}

	if (FFPISMRepresentationFragment* RepresentationFrag = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMRepresentationFragment>())
	{
		int Index = RepresentationFrag->ISMDescriptions.IndexOfByPredicate([&Guid](const FFPISMDescription& Desc)
		{
			return Desc.Guid == Guid;
		});

		if (Index >= 0)
		{
			RepresentationFrag->ISMDescriptions.RemoveAt(Index);
			return true;
		}
	}

	return false;
}

bool UFPMassBlueprintLibrary::SetEntityISMScale(const FMSEntityViewBPWrapper EntityHandle, FVector Scale)
{
	if (!EntityHandle.EntityView.GetEntity().IsValid())
	{
		return false;
	}

	if (FFPISMRepresentationFragment* RepresentationFrag = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMRepresentationFragment>())
	{
		RepresentationFrag->RelativeTransform.SetScale3D(Scale);
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

FFPISMDescription UFPMassBlueprintLibrary::GetEntityMainISMDescription(const FMSEntityViewBPWrapper EntityHandle)
{
	if (EntityHandle.EntityView.GetEntity().IsValid())
	{
		if (FFPISMRepresentationFragment* RepresentationFrag = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMRepresentationFragment>())
		{
			if (RepresentationFrag->ISMDescriptions.Num() > 0)
			{
				return RepresentationFrag->ISMDescriptions[0];
			}
		}
	}

	return FFPISMDescription();
}

bool UFPMassBlueprintLibrary::SetEntityMaxSpeed(const FMSEntityViewBPWrapper EntityHandle, float NewSpeed)
{
	if (!EntityHandle.EntityView.GetEntity().IsValid())
	{
		return false;
	}

	if (FFPSimpleMovementFragment* MovementFrag = EntityHandle.EntityView.GetFragmentDataPtr<FFPSimpleMovementFragment>())
	{
		MovementFrag->MaxSpeed = NewSpeed;
	}

	return true;
}
