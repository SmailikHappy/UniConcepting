// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SphereComponent.h"
#include "SlimeMoldRuntimeData.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNICONCEPTING_API USlimeMoldRuntimeData : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USlimeMoldRuntimeData();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TMap<TSharedPtr<USphereComponent>, TArray<int32>> TriggersToVertices;


};
