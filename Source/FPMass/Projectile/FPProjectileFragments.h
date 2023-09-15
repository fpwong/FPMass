// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassEntityTypes.h"
#include "ProjectileSim/Fragments/MSProjectileFragments.h"
#include "UObject/Object.h"
#include "FPProjectileFragments.generated.h"

class UFPProjectileEvents;

USTRUCT()
struct FFPMassTag_Projectile : public FMassTag
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FPMASS_API FFPProjectileStateFragment : public FMassFragment
{
	GENERATED_BODY()

	TArray<TWeakObjectPtr<AActor>> HitActors;
};

USTRUCT(BlueprintType)
struct FPMASS_API FFPProjectileCollisionParameters : public FMassSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TEnumAsByte<ECollisionChannel> Channel = ECC_Camera;
};

USTRUCT(BlueprintType)
struct FPMASS_API FFPProjectileEventsFragment : public FMassSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UFPProjectileEvents> ProjectileEvents;
};

UCLASS(meta = (DisplayName = "Pojectile Simulation"))
class FPMASS_API UFPProjectileTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	UPROPERTY(EditAnywhere)
	bool bRicochet = false;

	UPROPERTY(EditAnywhere)
	bool bHasGravity = true;

	UPROPERTY(EditAnywhere)
	bool bRotationFollowsVelocity = true;

	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties))
	FFPProjectileCollisionParameters CollisionParameters;

	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties))
	FFPProjectileEventsFragment ProjectileEvents;
};
