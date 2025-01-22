// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "WorldGeometryCache.generated.h"

UCLASS()
class SLIMEMOLD_API AWorldGeometryCache : public AActor
{
	GENERATED_BODY()

public:
	
	AWorldGeometryCache() {}

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<AActor*> GeometryActors;
};