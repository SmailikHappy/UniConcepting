// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"

#include "WorldGeometryCache.generated.h"

UCLASS()
class SLIMEMOLD_API UWorldGeometryCache : public UObject
{
	GENERATED_BODY()

public:
	
	UWorldGeometryCache() {}

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<AActor*> GeometryActors;
};