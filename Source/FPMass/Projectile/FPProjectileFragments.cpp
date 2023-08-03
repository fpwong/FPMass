// Fill out your copyright notice in the Description page of Project Settings.

#include "FPProjectileFragments.h"

#include "MassCommonFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassMovementFragments.h"
#include "FPMass/Miscellaneous/FPSimpleVelocityOrientationProcessor.h"

void UFPProjectileTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FFPProjectileStateFragment>();
	BuildContext.RequireFragment<FTransformFragment>();
	BuildContext.RequireFragment<FMassVelocityFragment>();
	BuildContext.AddTag<FFPMassTag_Projectile>();

	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	const FConstSharedStruct& CollisionFragment = EntityManager.GetOrCreateConstSharedFragment(CollisionParameters);
	BuildContext.AddConstSharedFragment(CollisionFragment);

	const FConstSharedStruct& ProjectileEventsFragment = EntityManager.GetOrCreateConstSharedFragment(ProjectileEvents);
	BuildContext.AddConstSharedFragment(ProjectileEventsFragment);

	if (bRicochet)
	{
		// BuildContext.AddTag<FMSProjectileRicochetTag>();
	}

	if(bHasGravity)
	{
		// TODO
		// BuildContext.AddTag<FMSGravityTag>();
	}

	if(bRotationFollowsVelocity)
	{
		BuildContext.AddTag<FFPMassTag_VelocityOrientation>();
	}

}
