// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassEntityTypes.h"
#include "FPAbilitySystemFragments.generated.h"

class UAbilitySystemComponent;

USTRUCT()
struct FPMASS_API FFPAbilitySystemFragment : public FMassFragment
{
	GENERATED_BODY()

	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystem;
};

UCLASS()
class FPMASS_API UFPAbilitySystemTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
