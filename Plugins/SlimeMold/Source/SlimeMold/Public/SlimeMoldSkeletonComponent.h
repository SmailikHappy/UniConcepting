// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include "Structs.h"


#include "SlimeMoldSkeletonComponent.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGenerateMeshEvent, UObject*, Properties);

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class SLIMEMOLD_API USlimeMoldSkeletonComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
	TArray<FSkeletonPoint> SkeletonPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
	TArray<FSkeletonLine> SkeletonLines;


	UPROPERTY(BlueprintAssignable, EditDefaultsOnly)
	FGenerateMeshEvent OnGenerateMesh;
};