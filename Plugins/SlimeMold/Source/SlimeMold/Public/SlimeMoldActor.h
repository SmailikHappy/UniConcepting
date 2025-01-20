// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "SlimeMoldActor.generated.h"

UCLASS()
class SLIMEMOLD_API ASlimeMoldActor : public AActor
{
	GENERATED_BODY()

public:
	
	ASlimeMoldActor();

	static bool EnableDebugDrawing;
	static float LineWidth;
	static float LineThickness;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Splines")
	TArray<USplineComponent*> SplineComponents;

	void AddSplineComponent(USplineComponent* SplineComponent);

	const TArray<USplineComponent*>& GetSplineComponents() const;

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
};