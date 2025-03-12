// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Structs.h"
#include "SlimeMoldBase.h"
#include "SlimeMoldEditorToolFunctionLibrary.generated.h"


/**
 * Static functions for the editor tool 
 * NOT callable from blueprints, cause this module is not available in runtime
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API USlimeMoldEditorFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static bool SingleSlimeMoldObjectIsSelected();
	static ASlimeMoldBase* GetSingleSelectedSlimeMoldObject();

	static void GetPoints(ASlimeMoldBase* SlimeMold, TArray<USkeletonPoint*>& Array) { Array = SlimeMold->SkeletonPoints; }
};
