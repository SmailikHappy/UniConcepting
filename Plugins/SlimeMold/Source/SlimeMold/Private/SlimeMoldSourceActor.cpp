// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlimeMoldSourceActor.h"

ASlimeMoldSource::ASlimeMoldSource()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(FName("AMoveableRigidBodyActor_SceneComponent"));
	SetRootComponent(SceneComponent);

	PrimaryActorTick.bCanEverTick = true;
}

void ASlimeMoldSource::BeginPlay()
{
	Super::BeginPlay();
}

void ASlimeMoldSource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}