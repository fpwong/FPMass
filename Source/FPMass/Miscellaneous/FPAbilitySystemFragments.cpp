// Fill out your copyright notice in the Description page of Project Settings.

#include "FPAbilitySystemFragments.h"

#include "AbilitySystemGlobals.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntityView.h"

void UFPAbilitySystemTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FFPAbilitySystemFragment>();

	BuildContext.GetMutableObjectFragmentInitializers().Add([=](UObject& Owner, FMassEntityView& EntityView, const EMassTranslationDirection CurrentDirection)
	{
		if (AActor* AsActor = Cast<AActor>(&Owner))
		{
			FFPAbilitySystemFragment& AbilitySystem = EntityView.GetFragmentData<FFPAbilitySystemFragment>();
			AbilitySystem.AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AsActor);
		}
	});
}
