// Fill out your copyright notice in the Description page of Project Settings.

#include "FPMassAgentComponent.h"

void UFPMassAgentComponent::OnRegister()
{
	if (MassConfigAsset)
	{
		SetEntityConfig(MassConfigAsset->GetConfig());
	}

	Super::OnRegister();
}
