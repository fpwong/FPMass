// Fill out your copyright notice in the Description page of Project Settings.

#include "FPMassBlueprintLibrary.h"

#include "FPAbilitySystemFragments.h"
#include "FPSlideMovementProcessor.h"
#include "GameplayTagContainer.h"
#include "Common/Misc/MSBPFunctionLibrary.h"
#include "FPMass/ISMRepresentation/FPISMRepresentationFragments.h"

void UFPMassBlueprintLibrary::PlayISMAnimation(FMSEntityViewBPWrapper EntityHandle, int AnimIndex, FFPISMAnimationCallbacks AnimationCallbacks)
{
	// const FFPISMParameters& ISMParameters = EntityHandle.EntityView.GetConstSharedFragmentData<FFPISMParameters>();
	if (const FFPISMRepresentationFragment* Representation = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMRepresentationFragment>())
	{
		if (FFPISMAnimationFragment* AnimState = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMAnimationFragment>())
		{
			Representation->ForEachActiveISMDescription([&](const FFPISMDescription& ISMDesc)
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
			});
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

FGuid UFPMassBlueprintLibrary::AddEntityISMLayer(const FMSEntityViewBPWrapper EntityHandle, FGameplayTag LayerTag, FFPISMDescription Description)
{
	if (EntityHandle.EntityView.GetEntity().IsValid())
	{
		if (FFPISMRepresentationFragment* RepresentationFrag = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMRepresentationFragment>())
		{
			return RepresentationFrag->AddLayer(LayerTag, Description);
		}
	}

	return FGuid();
}

bool UFPMassBlueprintLibrary::RemoveEntityISMLayer(const FMSEntityViewBPWrapper EntityHandle, FGameplayTag LayerTag, FGuid Guid)
{
	if (EntityHandle.EntityView.GetEntity().IsValid())
	{
		if (FFPISMRepresentationFragment* RepresentationFrag = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMRepresentationFragment>())
		{
			auto& Layers = RepresentationFrag->Layers.FindOrAdd(LayerTag).Layers;

			auto IndexToRemove = Layers.IndexOfByPredicate([Guid](const FFPISMDescriptionLayer& Layer)
			{
				return Layer.Guid == Guid;
			});

			// UE_LOG(LogTemp, Warning, TEXT("Remove %d"), IndexToRemove);

			if (IndexToRemove >= 0)
			{
				Layers.RemoveAt(IndexToRemove);
				return true;
			}
		}
	}

	return false;
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
