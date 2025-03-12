// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <DynamicMeshActor.h>
#include <UDynamicMesh.h>
#include "Structs.h"

#include "SlimeMoldBase.generated.h"
UCLASS()
class SLIMEMOLD_API ASlimeMoldBase : public ADynamicMeshActor
{
	GENERATED_BODY()

public:
	
	ASlimeMoldBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USkeletonPoint*> SkeletonPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSkeletonLine> SkeletonLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletonPoint* SkeletonPoint1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletonPoint* SkeletonPoint2;

protected:

	virtual void BeginPlay() override;


public:
	// The user should implement the generation of the mesh
	UFUNCTION(BlueprintImplementableEvent)
	void GenerateMesh(UObject* UserProperties);

	virtual void Tick(float DeltaTime) override;
};