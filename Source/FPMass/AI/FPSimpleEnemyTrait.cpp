// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSimpleEnemyTrait.h"

#include "FPAIFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "FPMass/ISMRepresentation/FPISMRepresentationTrait.h"
#include "FPMass/Miscellaneous/FPSlideMovementProcessor.h"

void UFPSimpleEnemyTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FFPTargetDataFragment>();

	BuildContext.AddFragment<FFPSimpleEnemyStateFragment>();

	BuildContext.AddFragment<FFPSimpleMovementFragment>();

	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);
	FConstSharedStruct& SimpleEnemyFragment = EntityManager.GetOrCreateConstSharedFragment(SimpleEnemyParameters);
	BuildContext.AddConstSharedFragment(SimpleEnemyFragment);
}
