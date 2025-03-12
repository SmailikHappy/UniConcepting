// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SlimeMoldBase.h"
#include "Structs.h"
#include "SlimeMoldRuntimeFunctionLibrary.generated.h"


/**
 * Static functions for SlimeMold mosule 
 * Callable from blueprints and available in runtime
 */

UCLASS()
class SLIMEMOLD_API USlimeMoldRuntimeBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	// Workaround for getting the values of the USkeletonPoint objects
	// For some reason if you try to access the USkeletonPoint directly from the FSkeletonLine struct
	// (or even from TArray of SkeletonPoints in the SlimeMold actor) in blueprints
	// it returns "variable not in scope"
	UFUNCTION(BlueprintCallable, Category = "SlimeMold")
	static void GetPointValues(const FSkeletonLine& FSkeletonLine, FVector& Point1Position, FVector& Point2Position);
};