#pragma once

#include "CoreMinimal.h"

#include "Common/Misc/MSBPFunctionLibrary.h"

#include "FPProjectileEvents.generated.h"

UCLASS(Blueprintable, meta = (ShowWorldContextPin), hidecategories = (Replication))
class FPMASS_API UFPProjectileEvents : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	bool OnBounce(UWorld* World, FMSEntityViewBPWrapper Entity, const FHitResult& HitResult) const;

	UFUNCTION(BlueprintImplementableEvent)
	bool OnHit(UWorld* World, FMSEntityViewBPWrapper Entity, const FHitResult& HitResult) const;
};
