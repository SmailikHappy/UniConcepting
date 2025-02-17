// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SlimeMoldEditorToolFunctionLibrary.generated.h"


class ASlimeMoldBase;

/**
 * Static functions for the editor tool
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API USlimeMoldEditorFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static bool SingleSlimeMoldObjectIsSelected();
	static ASlimeMoldBase* GetSingleSelectedSlimeMoldObject();
};
