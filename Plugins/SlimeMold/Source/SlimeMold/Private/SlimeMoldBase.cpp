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

//void ASlimeMoldBase::GenerateMesh(UObject* Properties)
//{
//	UE_LOG(LogTemp, Warning, TEXT("Base class GenerateMesh() function has been called"));
//}

void ASlimeMoldBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}