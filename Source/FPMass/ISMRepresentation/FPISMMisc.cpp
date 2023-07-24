// Fill out your copyright notice in the Description page of Project Settings.

#include "FPISMMisc.h"

#include "AnimToTextureDataAsset.h"

const FAnimInfo& FFPISMAnimation::GetAnimInfo() const
{
	check(IsValid());
	UAnimToTextureDataAsset* Data = AnimData.LoadSynchronous();
	return Data->Animations[AnimationIndex];
}

bool FFPISMAnimation::IsValid() const
{
	if (UAnimToTextureDataAsset* Data = AnimData.LoadSynchronous())
	{
		return Data->Animations.IsValidIndex(AnimationIndex);
	}

	return false;
}
