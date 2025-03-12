// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlimeMoldMeshEditingTool.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "BaseBehaviors/ClickDragBehavior.h"

// for raycast into World
#include "CollisionQueryParams.h"
#include "Engine/World.h"

#include "SceneManagement.h"
#include <Kismet/GameplayStatics.h>

// Custom static functions
#include "SlimeMoldEditorToolFunctionLibrary.h"


// localization namespace
#define LOCTEXT_NAMESPACE "USlimeMoldMeshEditingTool"

/*
 * Tool builder
 */

bool USlimeMoldMeshEditingToolBuilder::CanBuildTool(const FToolBuilderState& SceneState) const
{
	return USlimeMoldEditorFuncLib::SingleSlimeMoldObjectIsSelected();
}

UInteractiveTool* USlimeMoldMeshEditingToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	USlimeMoldMeshEditingTool* NewTool = NewObject<USlimeMoldMeshEditingTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

/*
 * Tool logic
 */

void USlimeMoldMeshEditingTool::Setup()
{
	TargetSlimeMoldActor = USlimeMoldEditorFuncLib::GetSingleSelectedSlimeMoldObject();
	check(TargetSlimeMoldActor);

	UInteractiveTool::Setup();

	// Add default mouse input behavior
	UClickDragInputBehavior* MouseBehavior = NewObject<UClickDragInputBehavior>();
	MouseBehavior->Initialize(this);
	AddInputBehavior(MouseBehavior);

	// Create the property set and register it with the Tool
	ToolProperties = NewObject<USlimeMoldMeshEditingToolProperties>(this);
	AddToolPropertySource(ToolProperties);

	ToolProperties->RestoreProperties(this, "MeshEditingToolProperties");

	if (ToolProperties->MeshPropertyClass)
	{
		// Create the property set and register it with the Tool
		MeshProperties = NewObject<USlimeMoldMeshPropertyBase>(this, ToolProperties->MeshPropertyClass);
		AddToolPropertySource(MeshProperties);

		UE_LOG(LogTemp, Warning, TEXT("Mesh properties were created"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Mesh editing tool has been initialized"));
}

void USlimeMoldMeshEditingTool::Shutdown(EToolShutdownType ShutdownType)
{
	switch (ShutdownType)
	{
	case EToolShutdownType::Completed:
		UE_LOG(LogTemp, Warning, TEXT("Tool was completed"));
		break;
	case EToolShutdownType::Accept:
		UE_LOG(LogTemp, Warning, TEXT("Tool was accepted"));
		break;
	case EToolShutdownType::Cancel:
		UE_LOG(LogTemp, Warning, TEXT("Tool was cancelled"));
		break;
	default:
		break;
	}

	//FMeshEditingToolBaseCustomization::MeshEditingToolBase.Reset();
}

void USlimeMoldMeshEditingTool::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}

FInputRayHit USlimeMoldMeshEditingTool::CanBeginClickDragSequence(const FInputDeviceRay& PressPos)
{
	// we only start drag if press-down is on top of something we can raycast
	FVector Temp;
	FInputRayHit Result = FindRayHit(PressPos.WorldRay, Temp);
	return Result;
}

void USlimeMoldMeshEditingTool::OnClickPress(const FInputDeviceRay& PressPos)
{
	// cast ray into world to find hit position
	FVector RayStart = PressPos.WorldRay.Origin;
	FVector RayEnd = PressPos.WorldRay.PointAt(99999999.f);
	FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllObjects);
	FHitResult Result;

	// Trace the ray and select the hit actor
	if (TargetWorld->LineTraceSingleByObjectType(Result, RayStart, RayEnd, QueryParams))
	{
		GEditor->SelectActor(Result.GetActor(), true, true);
	}
}

void USlimeMoldMeshEditingTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	if (!PropertySet) return;

	if (PropertySet == ToolProperties)
	{
		if (Property->GetName() == "bGenerateMesh")
		{
			if (!MeshProperties)
			{
				UE_LOG(LogTemp, Warning, TEXT("Mesh properties are not available"));
				return;
			}

			FEditorScriptExecutionGuard ScriptGuard;
			{
				UE_LOG(LogTemp, Warning, TEXT("Generating mesh"));
				TargetSlimeMoldActor->GenerateMesh(MeshProperties);
			}
		}

		if (Property->GetName() == "MeshPropertyClass")
		{
			// Reload the tool, so that MeshProperties with the chosen class would be generated in the panel 
			// For some reason we cannot add new properties to the tool in runtime
			// Huge thanks there is a workaround with a tool reloading
			ToolProperties->SaveProperties(this, "MeshEditingToolProperties");
			GetToolManager()->DeactivateTool(EToolSide::Left, EToolShutdownType::Completed);
			GetToolManager()->ActivateTool(EToolSide::Left);

			return;
		}
	}
}

FInputRayHit USlimeMoldMeshEditingTool::FindRayHit(const FRay& WorldRay, FVector& HitPos)
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

void USlimeMoldMeshEditingTool::Render(IToolsContextRenderAPI* RenderAPI)
{
	FPrimitiveDrawInterface* PDI = RenderAPI->GetPrimitiveDrawInterface();
}

#undef LOCTEXT_NAMESPACE
