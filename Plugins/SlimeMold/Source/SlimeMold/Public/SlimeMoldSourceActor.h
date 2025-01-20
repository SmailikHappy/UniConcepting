// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlimeMoldSourceActor.generated.h"

UCLASS()
class SLIMEMOLD_API ASlimeMoldSource : public AActor
{
	GENERATED_BODY()

public:
	
	ASlimeMoldSource();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Custom")
	USceneComponent* SceneComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Properties")
	float Radius;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Properties")
	int32 BranchCount;


protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
};