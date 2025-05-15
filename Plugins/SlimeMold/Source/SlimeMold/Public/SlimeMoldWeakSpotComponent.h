 // Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include "Structs.h"

#include "SlimeMoldWeakSpotComponent.generated.h"

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class SLIMEMOLD_API USlimeMoldWeakSpotComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
	FTransform WeakSpotSphereTransform = FTransform::Identity;
};