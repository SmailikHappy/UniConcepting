// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "WorldGeometryCache2.generated.h"

UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class SLIMEMOLD_API UWorldGeometryCache2 : public UObject
{
	GENERATED_BODY()

public:
	
	UWorldGeometryCache2() {}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AActor*> GeometryActors;
};