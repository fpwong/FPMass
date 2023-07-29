#include "FPEntitySystem.h"

#include "FPBubbleInfo.h"
#include "FPMassReplicationSettings.h"
#include "MassCommonFragments.h"
#include "MassEntityConfigAsset.h"
#include "MassEntitySubsystem.h"
#include "MassReplicationFragments.h"
#include "MassReplicationSubsystem.h"
#include "MassSpawnerTypes.h"
#include "Kismet/KismetSystemLibrary.h"

void UFPEntitySystem::ReplicatedSpawnEntity(UMassEntityConfigAsset* EntityConfig, const FTransform& Transform, int Count)
{
	const FMassEntityTemplate& EntityTemplate = EntityConfig->GetOrCreateEntityTemplate(*GetWorld());

	for (int i = 0; i < Count; ++i)
	{
		TArray<FInstancedStruct> FragmentStructs;
		FTransformFragment TransformData;

		FVector RabbitLocation = Transform.GetLocation();
		// RabbitLocation.X += i * 500;
		TransformData.SetTransform(FTransform(RabbitLocation));

		auto* ReplicationSubsystem = GetWorld()->GetSubsystem<UMassReplicationSubsystem>();
		// Need to manually set NetID because the UMassNetworkIDFragmentInitializer observer is not trigger by CreateEntity, only by BatchCreateEntities
		FMassNetworkIDFragment NetIdData;
		NetIdData.NetID = ReplicationSubsystem->GetNextAvailableMassNetID();
		FReplicationTemplateIDFragment TemplateIdData;
		TemplateIdData.ID = EntityTemplate.GetTemplateID();

		FragmentStructs.Emplace(FInstancedStruct::Make(TransformData));
		FragmentStructs.Emplace(FInstancedStruct::Make(NetIdData));
		FragmentStructs.Emplace(FInstancedStruct::Make(TemplateIdData));

		if (EntityTemplate.IsValid())
		{
			// TArray<FMassEntityHandle> Entities;
			// EntityManager->BatchCreateEntities(EntityTemplate.GetArchetype(), EntityTemplate.GetSharedFragmentValues(), Count, Entities);
			auto Entity = EntityManager->CreateEntity(EntityTemplate.GetArchetype(), EntityTemplate.GetSharedFragmentValues());
			EntityManager->SetEntityFragmentsValues(Entity, EntityTemplate.GetInitialFragmentValues());
			EntityManager->SetEntityFragmentsValues(Entity, FragmentStructs);
		}
	}
}

void UFPEntitySystem::Initialize(FSubsystemCollectionBase& Collection)
{
	auto* MassSubsystem = Collection.InitializeDependency<UMassEntitySubsystem>();
	check(MassSubsystem);

	EntityManager = MassSubsystem->GetMutableEntityManager().AsShared();
}

void UFPEntitySystem::Deinitialize()
{
	EntityManager.Reset();
}

void UFPEntitySystem::PostInitialize()
{
	auto* ReplicationSubsystem = UWorld::GetSubsystem<UMassReplicationSubsystem>(GetWorld());
	check(ReplicationSubsystem);

	ReplicationSubsystem->RegisterBubbleInfoClass(AFPUnitClientBubbleInfo::StaticClass());

	auto* SpawnerSubsystem = UWorld::GetSubsystem<UMassSpawnerSubsystem>(GetWorld());
	check(SpawnerSubsystem);

	for (TSoftObjectPtr<UMassEntityConfigAsset> MassEntityConfigAsset : GetMutableDefault<UFPMassReplicationSettings>()->PreloadedEntityConfigs)
	{
		if (UMassEntityConfigAsset* Loaded = MassEntityConfigAsset.LoadSynchronous())
		{
			UKismetSystemLibrary::PrintString(GetWorld(), "Initializing mass entity config");
			// UE_LOG(LogTemp, Warning, TEXT("Intiializing %s"), *Loaded->GetName());
			Loaded->GetOrCreateEntityTemplate(*GetWorld());
		}
	}

	// if (IsValid(UMSAssetManager::Get()->GameData))
	// {
	// 	if (IsValid(UMSAssetManager::Get()->GameData->UnitEntityConfig))
	// 	{
	// 		const auto& EntityTemplate = UMSAssetManager::Get()->GameData->UnitEntityConfig->GetOrCreateEntityTemplate(*GetWorld());
	// 		ensure(EntityTemplate.IsValid());
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Error, TEXT("Unit config invalid"));
	// 	}
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("Game data invalid"));
	// 	check(false);
	// }
}
