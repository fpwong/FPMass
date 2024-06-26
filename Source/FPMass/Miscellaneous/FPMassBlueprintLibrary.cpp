﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "FPMassBlueprintLibrary.h"

#include "FPAbilitySystemFragments.h"
#include "FPSlideMovementProcessor.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Common/Misc/MSBPFunctionLibrary.h"
#include "Elements/Actor/ActorElementLevelEditorViewportInteractionCustomization.h"
#include "FPMass/AI/FPSimpleEnemyProcessor.h"
#include "FPMass/ISMRepresentation/FPISMRepresentationFragments.h"

void UFPMassBlueprintLibrary::PlayISMAnimation(FMSEntityViewBPWrapper EntityHandle, int AnimIndex, FFPISMAnimationCallbacks AnimationCallbacks)
{
	if (!EntityHandle.IsValid())
	{
		return;
	}

	// const FFPISMParameters& ISMParameters = EntityHandle.EntityView.GetConstSharedFragmentData<FFPISMParameters>();
	if (const FFPISMRepresentationFragment* Representation = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMRepresentationFragment>())
	{
		if (FFPISMAnimationFragment* AnimState = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMAnimationFragment>())
		{
			Representation->ForEachActiveISMDescription([&](const FFPISMDescription& ISMDesc)
			{
				if (UFPAnimToTextureDataAsset* FPAnimToTexture = Cast<UFPAnimToTextureDataAsset>(ISMDesc.AnimToTextureData))
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
							NewAnimation.AnimSpeed = FPAnimToTexture->GetAnimSpeed(AnimIndex);

							AnimState->CurrentMontage = NewAnimation;
						}
					}
				}
			});
		}
	}
}

void UFPMassBlueprintLibrary::StopISMAnimation(FMSEntityViewBPWrapper EntityHandle)
{
	if (!EntityHandle.IsValid())
	{
		return;
	}

	if (const FFPISMRepresentationFragment* Representation = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMRepresentationFragment>())
	{
		if (FFPISMAnimationFragment* AnimState = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMAnimationFragment>())
		{
			Representation->ForEachActiveISMDescription([AnimState](const FFPISMDescription& ISMDesc)
			{
				if (ISMDesc.AnimToTextureData)
				{
					AnimState->CurrentMontage.Reset();
				}
			});
		}
	}
}

UAbilitySystemComponent* UFPMassBlueprintLibrary::GetAbilitySystemFromEntity(const FMSEntityViewBPWrapper EntityHandle)
{
	if (!EntityHandle.IsValid())
	{
		return nullptr;
	}

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

bool UFPMassBlueprintLibrary::ScaleUpEntity(const FMSEntityViewBPWrapper EntityHandle, FVector Scale)
{
	if (!EntityHandle.EntityView.GetEntity().IsValid())
	{
		return false;
	}

	if (FFPISMRepresentationFragment* RepresentationFrag = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMRepresentationFragment>())
	{
		const FVector NewScale = RepresentationFrag->RelativeTransform.GetScale3D() * Scale;
		RepresentationFrag->RelativeTransform.SetScale3D(NewScale);
	}

	return true;
}

bool UFPMassBlueprintLibrary::ScaleDownEntity(const FMSEntityViewBPWrapper EntityHandle, FVector Scale)
{
	if (!EntityHandle.EntityView.GetEntity().IsValid())
	{
		return false;
	}

	if (FFPISMRepresentationFragment* RepresentationFrag = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMRepresentationFragment>())
	{
		const FVector NewScale = RepresentationFrag->RelativeTransform.GetScale3D() / Scale;
		RepresentationFrag->RelativeTransform.SetScale3D(NewScale);
	}

	return true;
}

FVector UFPMassBlueprintLibrary::GetEntityISMScale(const FMSEntityViewBPWrapper EntityHandle)
{
	if (EntityHandle.EntityView.GetEntity().IsValid())
	{
		if (FFPISMRepresentationFragment* RepresentationFrag = EntityHandle.EntityView.GetFragmentDataPtr<FFPISMRepresentationFragment>())
		{
			return RepresentationFrag->RelativeTransform.GetScale3D();
		}
	}

	return FVector(1,1,1);
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

bool UFPMassBlueprintLibrary::SetEnemyAttackAbility(const FMSEntityViewBPWrapper EntityHandle, TSubclassOf<UGameplayAbility> Ability)
{
	if (EntityHandle.EntityView.GetEntity().IsValid())
	{
		if (FFPSimpleEnemyParameters* EnemyParams = EntityHandle.EntityView.GetFragmentDataPtr<FFPSimpleEnemyParameters>())
		{
			if (EnemyParams->AttackAbility != Ability)
			{
				EnemyParams->AttackAbility = Ability;
				return true;
			}
		}
	}

	return false;
}
