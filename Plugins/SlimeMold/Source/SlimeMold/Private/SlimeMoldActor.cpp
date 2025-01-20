// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlimeMoldActor.h"

bool ASlimeMoldActor::EnableDebugDrawing = false;
float ASlimeMoldActor::LineWidth = 500.0f;
float ASlimeMoldActor::LineThickness = 20.0f;

ASlimeMoldActor::ASlimeMoldActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASlimeMoldActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASlimeMoldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASlimeMoldActor::AddSplineComponent(USplineComponent* SplineComponent)
{	
	if (SplineComponent)
	{
		SplineComponents.Add(SplineComponent);
	}
}

const TArray<USplineComponent*>& ASlimeMoldActor::GetSplineComponents() const
{
	return SplineComponents;
}