// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSimpleEnemyTrait.h"

#include "FPAIFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "FPMass/ISMRepresentation/FPISMRepresentationTrait.h"

void UFPSimpleEnemyTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FFPISMAnimationFragment>();
	BuildContext.AddFragment<FFPTargetDataFragment>();

	BuildContext.AddFragment<FFPSimpleEnemyStateFragment>();

	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);
	FConstSharedStruct& SimpleEnemyFragment = EntityManager.GetOrCreateConstSharedFragment(SimpleEnemyParameters);
	BuildContext.AddConstSharedFragment(SimpleEnemyFragment);
}
