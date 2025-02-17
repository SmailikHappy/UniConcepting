// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeshEditingToolBase.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "BaseBehaviors/ClickDragBehavior.h"

// for raycast into World
#include "CollisionQueryParams.h"
#include "Engine/World.h"

#include "SceneManagement.h"
#include <Kismet/GameplayStatics.h>



// localization namespace
#define LOCTEXT_NAMESPACE "UMeshEditingToolBase"

/*
 * Tool builder
 */

UInteractiveTool* UMeshEditingToolBaseBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UMeshEditingToolBase* NewTool = NewObject<UMeshEditingToolBase>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

/*
 * Tool logic
 */

void UMeshEditingToolBase::Setup()
{
	UInteractiveTool::Setup();

	// Add default mouse input behavior
	UClickDragInputBehavior* MouseBehavior = NewObject<UClickDragInputBehavior>();
	MouseBehavior->Initialize(this);
	AddInputBehavior(MouseBehavior);

	// Create the property set and register it with the Tool
	Properties = NewObject<UMeshEditingToolBaseProperties>(this);
	AddToolPropertySource(Properties);

	UE_LOG(LogTemp, Warning, TEXT("This step has been passed"));

	// Register the custom detail customization
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");	
}

void UMeshEditingToolBase::Shutdown(EToolShutdownType ShutdownType)
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

void UMeshEditingToolBase::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}

FInputRayHit UMeshEditingToolBase::CanBeginClickDragSequence(const FInputDeviceRay& PressPos)
{
	// we only start drag if press-down is on top of something we can raycast
	FVector Temp;
	FInputRayHit Result = FindRayHit(PressPos.WorldRay, Temp);
	return Result;
}

void UMeshEditingToolBase::OnClickPress(const FInputDeviceRay& PressPos)
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

void UMeshEditingToolBase::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	if (!PropertySet) return;

	if (PropertySet != Properties) return;
}

FInputRayHit UMeshEditingToolBase::FindRayHit(const FRay& WorldRay, FVector& HitPos)
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

void UMeshEditingToolBase::Render(IToolsContextRenderAPI* RenderAPI)
{
	FPrimitiveDrawInterface* PDI = RenderAPI->GetPrimitiveDrawInterface();
}

#undef LOCTEXT_NAMESPACE
