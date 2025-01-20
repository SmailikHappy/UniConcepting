// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlimeMoldEditorToolExample.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "BaseBehaviors/ClickDragBehavior.h"

// Custom tool customizations
#include "SlimeMoldEditorToolExampleCustomization.h"

// for raycast into World
#include "CollisionQueryParams.h"
#include "Engine/World.h"

#include "SceneManagement.h"
#include <Kismet/GameplayStatics.h>

// localization namespace
#define LOCTEXT_NAMESPACE "USlimeMoldEditorToolExample"

/*
 * ToolBuilder
 */

UInteractiveTool* USlimeMoldEditorToolExampleBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	USlimeMoldEditorToolExample* NewTool = NewObject<USlimeMoldEditorToolExample>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}


void USlimeMoldEditorToolExample::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}

FInputRayHit USlimeMoldEditorToolExample::CanBeginClickDragSequence(const FInputDeviceRay& PressPos)
{
	// we only start drag if press-down is on top of something we can raycast
	FVector Temp;
	FInputRayHit Result = FindRayHit(PressPos.WorldRay, Temp);
	return Result;
}

void USlimeMoldEditorToolExample::Setup()
{
	UInteractiveTool::Setup();

	// Add default mouse input behavior
	UClickDragInputBehavior* MouseBehavior = NewObject<UClickDragInputBehavior>();
	MouseBehavior->Initialize(this);
	AddInputBehavior(MouseBehavior);

	// Create the property set and register it with the Tool
	Properties = NewObject<USlimeMoldEditorToolExampleProperties>(this);
	AddToolPropertySource(Properties);

	// Register the custom detail customization
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");	
	PropertyModule.RegisterCustomClassLayout(
		USlimeMoldEditorToolExampleProperties::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FSlimeMoldEditorToolExampleCustomization::MakeInstance)
	);
}


FInputRayHit USlimeMoldEditorToolExample::FindRayHit(const FRay& WorldRay, FVector& HitPos)
{
	// trace a ray into the World
	FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllObjects);
	FHitResult Result;
	bool bHitWorld = TargetWorld->LineTraceSingleByObjectType(Result, WorldRay.Origin, WorldRay.PointAt(999999), QueryParams);
	if (bHitWorld)
	{
		HitPos = Result.ImpactPoint;
		return FInputRayHit(Result.Distance);
	}
	return FInputRayHit();
}


#undef LOCTEXT_NAMESPACE
