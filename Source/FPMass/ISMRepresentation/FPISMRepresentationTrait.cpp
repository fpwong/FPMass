// Fill out your copyright notice in the Description page of Project Settings.

#include "FPISMRepresentationTrait.h"

#include "FPISMSubsystem.h"
#include "MassCommonFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntityView.h"
#include "Components/InstancedStaticMeshComponent.h"

TArray<float> FFPISMAnimationFragment::AsCustomData() const
{
	float IdleWalkFrame = 0;
	float MontageCurrFrame = 0;
	float MontageStartFrame = 0;
	float MontageNumFrames = 0;

	if (CurrentMontage.IsSet())
	{
		MontageCurrFrame = CurrentMontage->CurrentFrame;
		MontageStartFrame = CurrentMontage->StartFrame;
		MontageNumFrames = CurrentMontage->NumFrames;
	}

	return { WalkBlend, MontageBlend, IdleWalkFrame, MontageCurrFrame, MontageStartFrame, MontageNumFrames };
}

void UFPISMRepresentationTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	if (!World.IsGameWorld())
	{
		return;
	}

	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	// if (!StaticMesh)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Forgot to set static mesh on FPISMRepresentationTrait"));
	// 	return;
	// }

	// Evil main thread loads
	// StaticMesh.LoadSynchronous();

	// UStaticMesh* SM = StaticMeshSharedFragment.StaticMesh.LoadSynchronous();
	// if (!SM)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("SM Invalid!"));
	// 	return;
	// }

	const FConstSharedStruct& SharedStaticMeshFragment = EntityManager.GetOrCreateConstSharedFragment(StaticMeshSharedFragment);
	BuildContext.AddConstSharedFragment(SharedStaticMeshFragment);

	BuildContext.RequireFragment<FTransformFragment>();

	BuildContext.AddFragment<FFPISMInstanceIdFragment>();

	// BuildContext.GetMutableObjectFragmentInitializers().Add([=](UObject& Owner, FMassEntityView& EntityView, const EMassTranslationDirection CurrentDirection)
	// {
	// 	const auto& ISMSharedFragment = EntityView.GetConstSharedFragmentData<FFPISMSharedFragment>();
	// 	if (UStaticMesh* SM = ISMSharedFragment.StaticMesh.LoadSynchronous())
	// 	{
	// 		if (UFPISMSubsystem* ISMSubsystem = UWorld::GetSubsystem<UFPISMSubsystem>(Owner.GetWorld()))
	// 		{
	// 			if (AFPISMActor* ISMActor = ISMSubsystem->FindOrCreateISM(SM))
	// 			{
	// 				FFPISMActorSharedFragment SharedISMActorFrag;
	// 				SharedISMActorFrag.ISMActor = ISMActor;
	// 				const FSharedStruct& SharedFrag = EntityManager.GetOrCreateSharedFragmentByHash<FFPISMActorSharedFragment>(SM->GetUniqueID(), SharedISMActorFrag);
	// 				BuildContext.AddSharedFragment(SharedFrag);
	// 			}
	// 		}
	// 	}
	//
	// // 	// FFPISMInstanceIdFragment& InstanceIdFragment = EntityView.GetFragmentData<FFPISMInstanceIdFragment>();
	// // 	//
	// // 	// const FTransform& Transform = EntityView.GetFragmentData<FTransformFragment>().GetTransform();
	// // 	// const FFPISMSharedFragment& ISMSharedFragment = EntityView.GetConstSharedFragmentData<FFPISMSharedFragment>();
	// // 	//
	// // 	// UFPISMSubsystem* ISMSubsystem = UWorld::GetSubsystem<UFPISMSubsystem>(Owner.GetWorld());
	// // 	// if (UInstancedStaticMeshComponent* ISMComponent = ISMSubsystem->FindOrCreateISM(ISMSharedFragment.StaticMesh.LoadSynchronous()))
	// // 	// {
	// // 	// 	InstanceIdFragment.InstanceId = ISMComponent->AddInstance(Transform, true);
	// // 	// 	InstanceIdFragment.PrevTransform = Transform;
	// // 	// 	UE_LOG(LogTemp, Warning, TEXT("Made ISM %d"), InstanceIdFragment.InstanceId);
	// // 	// }
	// //
	// // 	// AbilitySysFrag.AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AsActor);
	// });
	// FSharedStruct SharedFragment =
	// 	ISMSubsystem->GetOrCreateSharedNiagaraFragmentForSystemType(SharedNiagaraSystem.Get(), StaticMesh.Get(), MaterialOverride.Get());

	// BuildContext.AddSharedFragment(SharedFragment);
}
