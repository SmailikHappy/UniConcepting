// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs.h"

#include "SlimeMoldBase.generated.h"

UCLASS()
class SLIMEMOLD_API ASlimeMoldBase : public AActor
{
	GENERATED_BODY()

public:
	
	ASlimeMoldBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USkeletonPoint*> SkeletonPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSkeletonLine> SkeletonLines;

protected:

	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void GenerateMesh();

	virtual void Tick(float DeltaTime) override;
};