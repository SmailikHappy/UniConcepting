// Copyright Epic Games, Inc. All Rights Reserved.



#include "SlimeMoldEditorToolSourceManaging.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "BaseBehaviors/ClickDragBehavior.h"

// Custom tool customizations
#include "SlimeMoldEditorToolSourceManagingCustomization.h"

// for raycast into World
#include "CollisionQueryParams.h"
#include "Engine/World.h"

#include "SceneManagement.h"
#include <Kismet/GameplayStatics.h>

#define LOCTEXT_NAMESPACE "USlimeMoldEditorToolSourceManaging"


/*
 * Tool builder
 */

UInteractiveTool* USlimeMoldEditorToolSourceManagingBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	USlimeMoldEditorToolSourceManaging* NewTool = NewObject<USlimeMoldEditorToolSourceManaging>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

/*
 * Tool logic
 */

void USlimeMoldEditorToolSourceManaging::Setup()
{
	UInteractiveTool::Setup();

	// Add default mouse input behavior
	UClickDragInputBehavior* MouseBehavior = NewObject<UClickDragInputBehavior>();
	MouseBehavior->Initialize(this);
	AddInputBehavior(MouseBehavior);

	// Create the property set and register it with the Tool
	Properties = NewObject<USlimeMoldEditorToolSourceManagingProperties>(this);
	AddToolPropertySource(Properties);

	// Register the custom detail customization
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomClassLayout(
		USlimeMoldEditorToolSourceManagingProperties::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FSlimeMoldEditorToolSourceManagingCustomization::MakeInstance)
	);

	FSlimeMoldEditorToolSourceManagingCustomization::SlimeMoldEditorToolSourceManaging = MakeShareable(this);

	SetupGizmo();
}

void USlimeMoldEditorToolSourceManaging::SetupGizmo()
{
	UInteractiveGizmoManager* const GizmoManager = GetToolManager()->GetPairedGizmoManager();
	ensure(GizmoManager);

	TransformProxy = NewObject<UTransformProxy>(this);
	ensure(TransformProxy);

	TransformGizmo = GizmoManager->Create3AxisTransformGizmo();
	ensure(TransformGizmo);

	TransformProxy->OnTransformChanged.AddWeakLambda(this, [this](UTransformProxy*, FTransform NewTransform)
		{
			if (CurrentSlimeMoldSource)
			{
				CurrentSlimeMoldSource->SetActorTransform(NewTransform);
			}
		});

	TransformGizmo->SetVisibility(false);
	TransformGizmo->bUseContextCoordinateSystem = false;
	TransformGizmo->bUseContextGizmoMode = false;
	TransformGizmo->CurrentCoordinateSystem = EToolContextCoordinateSystem::World;
}

//void USlimeMoldEditorToolSourceManaging::Shutdown(EToolShutdownType ShutdownType)
//{
//	switch (ShutdownType)
//	{
//	case EToolShutdownType::Completed:
//		UE_LOG(LogTemp, Warning, TEXT("Tool was completed"));
//		break;
//	case EToolShutdownType::Accept:
//		UE_LOG(LogTemp, Warning, TEXT("Tool was accepted"));
//		break;
//	case EToolShutdownType::Cancel:
//		UE_LOG(LogTemp, Warning, TEXT("Tool was cancelled"));
//		break;
//	default:
//		break;
//	}
//}

void USlimeMoldEditorToolSourceManaging::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}

FInputRayHit USlimeMoldEditorToolSourceManaging::CanBeginClickDragSequence(const FInputDeviceRay& PressPos)
{
	// we only start drag if press-down is on top of something we can raycast
	FVector Temp;
	FInputRayHit Result = FindRayHit(PressPos.WorldRay, Temp);
	return Result;
}

void USlimeMoldEditorToolSourceManaging::OnClickPress(const FInputDeviceRay& PressPos)
{
	FVector ClickLocation;
	FInputRayHit HitResult = FindRayHit(PressPos.WorldRay, ClickLocation);

	if (!HitResult.bHit) {
		UE_LOG(LogTemp, Warning, TEXT("Void has been clicked"));
		return;
	}

	if (CurrentSlimeMoldSource) {
		// Simply ignore this click, as we are controlling over the slime mold source that was spawned previously
		return;
	}

	// Spawn the slime mold source
	FTransform SlimeMoldSourceTransform(FRotator::ZeroRotator, ClickLocation);

	CurrentSlimeMoldSource = TargetWorld->SpawnActor<ASlimeMoldSource>(ASlimeMoldSource::StaticClass(), SlimeMoldSourceTransform, FActorSpawnParameters());

	FName ActorName = MakeUniqueObjectName(TargetWorld, ASlimeMoldSource::StaticClass(), FName(TEXT("SlimeMoldSource")));
	CurrentSlimeMoldSource->SetActorLabel(ActorName.ToString());

	// Update defaults
	CurrentSlimeMoldSource->Radius = Properties->Radius;
	CurrentSlimeMoldSource->BranchCount = Properties->BranchCount;

	// Attach gizmo
	TransformProxy->SetTransform(SlimeMoldSourceTransform);
	TransformGizmo->SetActiveTarget(TransformProxy, GetToolManager());

	TransformGizmo->SetVisibility(true);
}

FReply USlimeMoldEditorToolSourceManaging::AcceptSlimeMoldSource()
{
	// Set up gizmo
	TransformGizmo->SetActiveTarget(TransformProxy, GetToolManager());

	TransformGizmo->SetVisibility(false);

	CurrentSlimeMoldSource = nullptr;

	return FReply::Handled();
}

void USlimeMoldEditorToolSourceManaging::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	if (!PropertySet) return;

	if (PropertySet != Properties) return;

	if (!CurrentSlimeMoldSource) return;

	CurrentSlimeMoldSource->Radius = Properties->Radius;
	CurrentSlimeMoldSource->BranchCount = Properties->BranchCount;
}

FInputRayHit USlimeMoldEditorToolSourceManaging::FindRayHit(const FRay& WorldRay, FVector& HitPos)
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

void USlimeMoldEditorToolSourceManaging::Render(IToolsContextRenderAPI* RenderAPI)
{
	FPrimitiveDrawInterface* PDI = RenderAPI->GetPrimitiveDrawInterface();

	if (!CurrentSlimeMoldSource) return;

	// Workaround, idk how to render sphere mesh
	DrawDebugSphere(TargetWorld, CurrentSlimeMoldSource->GetActorLocation(), Properties->Radius, 12, FColor::Red, false, 0.1f);
}

#undef LOCTEXT_NAMESPACE
