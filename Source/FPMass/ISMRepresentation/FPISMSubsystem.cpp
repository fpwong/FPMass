#include "FPISMSubsystem.h"

#include "FPISMActor.h"
#include "MassAgentSubsystem.h"
#include "MassEntitySubsystem.h"
#include "MassSimulationSubsystem.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Materials/MaterialInstance.h"

void UFPISMSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency(UMassSimulationSubsystem::StaticClass());
	Collection.InitializeDependency(UMassActorSpawnerSubsystem::StaticClass());
	Collection.InitializeDependency(UMassAgentSubsystem::StaticClass());
	Super::Initialize(Collection);

	UMassEntitySubsystem* MassSubsystem = Collection.InitializeDependency<UMassEntitySubsystem>();
	MassManager = MassSubsystem->GetMutableEntityManager().AsShared();

	if (UWorld* World = GetWorld())
	{
		UMassSimulationSubsystem* SimSystem = World->GetSubsystem<UMassSimulationSubsystem>();
		check(SimSystem);
		SimSystem->GetOnProcessingPhaseStarted(EMassProcessingPhase::PrePhysics).AddUObject(this, &UFPISMSubsystem::OnProcessingPhaseStarted, EMassProcessingPhase::PrePhysics);
		SimSystem->GetOnProcessingPhaseFinished(EMassProcessingPhase::PostPhysics).AddUObject(this, &UFPISMSubsystem::OnProcessingPhaseStarted, EMassProcessingPhase::PostPhysics);
	}
}

AFPISMActor* UFPISMSubsystem::FindOrCreateISM(UStaticMesh* StaticMesh)
{
	if (StaticMesh)
	{
		const uint32 MeshId = StaticMesh->GetUniqueID();
		if (ISMActors.Contains(StaticMesh->GetUniqueID()))
		{
			return ISMActors[MeshId];
		}

		if (AFPISMActor* Actor = GetWorld()->SpawnActor<AFPISMActor>())
		{
			// UE_LOG(LogTemp, Warning, TEXT("Spawned ISMActor"));
			Actor->InstancedStaticMesh->SetStaticMesh(StaticMesh);
			ISMActors.Add(MeshId, Actor);
			return Actor;
		}
	}

	return nullptr;
}

AFPISMActor* UFPISMSubsystem::FindOrCreateISM(const FFPISMDescription& Desc)
{
	if (!Desc.AnimToTextureData)
	{
		UE_LOG(LogTemp, Error, TEXT("No Anim To Texture Data"));
		return nullptr;
	}

	if (!Desc.AnimToTextureData->GetStaticMesh())
	{
		UE_LOG(LogTemp, Error, TEXT("No static mesh set for %s"), *Desc.AnimToTextureData->GetName());
		return nullptr;
	}

	UStaticMesh* Mesh = Desc.AnimToTextureData->GetStaticMesh();

	const uint32 Hash = GetTypeHash(Desc);
	if (ISMActors.Contains(Hash))
	{
		return ISMActors[Hash];
	}

	if (AFPISMActor* Actor = GetWorld()->SpawnActor<AFPISMActor>())
	{
		// UE_LOG(LogTemp, Warning, TEXT("Spawned ISMActor"));
		Actor->InstancedStaticMesh->SetStaticMesh(Mesh);

		for (int32 ElementIndex = 0; ElementIndex < Desc.MaterialOverrides.Num(); ++ElementIndex)
		{
			if (UMaterialInterface* MaterialOverride = Desc.MaterialOverrides[ElementIndex])
			{
				Actor->InstancedStaticMesh->SetMaterial(ElementIndex, MaterialOverride);
			}
		}

		ISMActors.Add(Hash, Actor);
		return Actor;
	}

	return nullptr;
}

void UFPISMSubsystem::OnProcessingPhaseStarted(const float DeltaSeconds, const EMassProcessingPhase Phase) const
{
	switch (Phase)
	{
		case EMassProcessingPhase::PrePhysics:
			BeginVisualChanges();
			break;
		case EMassProcessingPhase::PostPhysics: /* Currently this is the end of phases signal */
			EndVisualChanges();
			break;
		default:
			check(false); // Need to handle this case
			break;
	}
}

void UFPISMSubsystem::BeginVisualChanges() const
{
	for (auto& Elem : ISMActors)
	{
		FFPISMData& SharedData = Elem.Value->SharedData;
		// UE_LOG(LogTemp, Warning, TEXT("BEGIN %d"), SharedData.UpdateInstanceIds.Num());

		SharedData.UpdateInstanceIds.Reset();
		SharedData.StaticMeshInstanceCustomFloats.Reset();
		SharedData.StaticMeshInstanceTransforms.Reset();
		SharedData.StaticMeshInstancePrevTransforms.Reset();
		SharedData.WriteIterator = 0;
	}
}

void UFPISMSubsystem::EndVisualChanges() const
{
	for (auto& Elem : ISMActors)
	{
		FFPISMData& SharedData = Elem.Value->SharedData;
		UInstancedStaticMeshComponent* ISMComp = Elem.Value->InstancedStaticMesh;
		ISMComp->UpdateInstances(SharedData.UpdateInstanceIds, SharedData.StaticMeshInstanceTransforms, SharedData.StaticMeshInstancePrevTransforms, 6, SharedData.StaticMeshInstanceCustomFloats);
		// UE_LOG(LogTemp, Warning, TEXT("END %d"), SharedData.UpdateInstanceIds.Num());
	}
}

// FSharedStruct UFPISMSubsystem::GetOrCreateSharedNiagaraFragmentForSystemType(UNiagaraSystem* NiagaraSystem, UStaticMesh* StaticMeshOverride, UMaterialInterface* MaterialOverride)
// {
// 	// We only want to key these based off of unique types of niagara systems! Usually the entire fragment would be hashed.
//
// 	// Probably a more sensible way to get a consistent hash?
// 	uint32 NiagaraAssetHash = GetTypeHash(NiagaraSystem->GetPathName());
// 	uint32 ParamsHash = NiagaraAssetHash;
// 	if (StaticMeshOverride)
// 	{
// 		ParamsHash = HashCombineFast(NiagaraAssetHash, GetTypeHash(StaticMeshOverride->GetFName()));
// 	}
// 	if (MaterialOverride)
// 	{
// 		ParamsHash = HashCombineFast(NiagaraAssetHash, GetTypeHash(MaterialOverride->GetFName()));
// 	}
// 	// FSharedNiagaraSystemFragment SharedStructToReturn;
//
// 	//try to see if we have seen this system type before...
// 	if (PreexistingSharedNiagaraActors.Contains(ParamsHash))
// 	{
// 		//if yes, just grab the one we made earlier!
// 		return MassManager->GetOrCreateSharedFragmentByHash<FSharedNiagaraSystemFragment>(ParamsHash, SharedStructToReturn);
// 	}
//
// 	FActorSpawnParameters SpawnParameters;
//
// 	SpawnParameters.ObjectFlags = RF_Transient | RF_DuplicateTransient;
//
// 	//if not, we need to spawn an entity+actor for it!
// 	AMSNiagaraActor* NewNiagaraActor = GetWorld()->SpawnActor<AMSNiagaraActor>(SpawnParameters);
//
// 	// We need this to tick last so that it receives the new gameplay state we create in the mass manager (stuff moving etc) for the next frame.
// 	NewNiagaraActor->GetNiagaraComponent()->SetTickBehavior(ENiagaraTickBehavior::ForceTickLast);
// 	NewNiagaraActor->GetNiagaraComponent()->SetAsset(NiagaraSystem);
//
// 	if (StaticMeshOverride)
// 	{
// 		NewNiagaraActor->GetNiagaraComponent()->SetVariableStaticMesh("StaticMeshToRender", StaticMeshOverride);
//
// 		if (MaterialOverride)
// 		{
// 			NewNiagaraActor->GetNiagaraComponent()->SetVariableMaterial("StaticMeshMaterial", MaterialOverride);
// 		}
// 		else
// 		{
// 			NewNiagaraActor->GetNiagaraComponent()->SetVariableMaterial("StaticMeshMaterial", StaticMeshOverride->GetMaterial(0));
// 		}
// 	}
// 	SharedStructToReturn.NiagaraManagerActor = NewNiagaraActor;
//
// 	PreexistingSharedNiagaraActors.FindOrAdd(ParamsHash, NewNiagaraActor);
//
// 	return MassManager->GetOrCreateSharedFragmentByHash<FSharedNiagaraSystemFragment>(ParamsHash, SharedStructToReturn);
// }
