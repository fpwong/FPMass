// Fill out your copyright notice in the Description page of Project Settings.

#include "FPMassAgentComponent.h"

#include "MassEntitySubsystem.h"

UFPMassAgentComponent::UFPMassAgentComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

bool UFPMassAgentComponent::IsAgentValid()
{
	const FMassEntityManager& EntityManager = GetWorld()->GetSubsystem<UMassEntitySubsystem>()->GetEntityManager();

	if (!IsEntityPendingCreation())
	{
		if (AgentHandle.IsValid() && EntityManager.GetArchetypeForEntity(AgentHandle).IsValid())
		{
			return true;
		}
	}

	return false;
}

void UFPMassAgentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// TODO should hook into PuppetInitializationDone or broadcast state changes but they aren't virtual 
	if (bWaitingInitialized && IsAgentValid())
	{
		OnMassAgentInitialized.Broadcast();
		SetComponentTickEnabled(false);
		bWaitingInitialized = false;
	}
}

void UFPMassAgentComponent::OnRegister()
{
	if (MassConfigAsset)
	{
		SetEntityConfig(MassConfigAsset->GetConfig());
	}

	Super::OnRegister();
}
