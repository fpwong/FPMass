// Fill out your copyright notice in the Description page of Project Settings.

#include "FPISMActor.h"

#include "Components/InstancedStaticMeshComponent.h"

AFPISMActor::AFPISMActor()
{
	InstancedStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("InstancedStaticMesh");
	InstancedStaticMesh->NumCustomDataFloats = 6;
	InstancedStaticMesh->SetGenerateOverlapEvents(false);
	InstancedStaticMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	SetRootComponent(InstancedStaticMesh);
}
