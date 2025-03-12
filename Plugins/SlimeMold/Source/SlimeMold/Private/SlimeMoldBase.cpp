// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlimeMoldBase.h"

ASlimeMoldBase::ASlimeMoldBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASlimeMoldBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASlimeMoldBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}