// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSimpleEnemyTrait.h"

#include "FPAIFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "Abilities/GameplayAbility.h"
#include "FPMass/ISMRepresentation/FPISMRepresentationTrait.h"
#include "FPMass/Miscellaneous/FPSlideMovementProcessor.h"

void UFPSimpleEnemyTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FFPTargetDataFragment>();

	BuildContext.AddFragment<FFPSimpleEnemyStateFragment>();

	BuildContext.AddFragment<FFPSimpleMovementFragment>();

	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	// TODO I think this screw up if we have another shared frag which uses this TemplateID...
	const uint32 Hash = BuildContext.GetTemplateID().GetHash64();

	const FSharedStruct& SimpleEnemyFragment = EntityManager.GetOrCreateSharedFragmentByHash<FFPSimpleEnemyParameters>(Hash, SimpleEnemyParameters);

	BuildContext.AddSharedFragment(SimpleEnemyFragment);
}
