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

	const uint32 Hash = UE::StructUtils::GetStructCrc32(FConstStructView::Make(SimpleEnemyParameters));
	const FSharedStruct& SimpleEnemyFragment = EntityManager.GetOrCreateSharedFragmentByHash<FFPSimpleEnemyParameters>(Hash, SimpleEnemyParameters);
	BuildContext.AddSharedFragment(SimpleEnemyFragment);
}
