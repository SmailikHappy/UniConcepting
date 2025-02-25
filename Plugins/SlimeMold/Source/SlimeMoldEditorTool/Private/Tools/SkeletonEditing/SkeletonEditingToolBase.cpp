// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkeletonEditingToolBase.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "BaseBehaviors/SingleClickBehavior.h"
#include "BaseBehaviors/ClickDragBehavior.h"
#include "BaseBehaviors/MouseHoverBehavior.h"


// for raycast into World
#include "CollisionQueryParams.h"
#include "Engine/World.h"

#include "SceneManagement.h"
#include <Kismet/GameplayStatics.h>



// localization namespace
#define LOCTEXT_NAMESPACE "USkeletonEditingToolBase"

/*
 * Tool logic
 */

void USkeletonEditingToolBase::Setup()
{
	UInteractiveTool::Setup();

	UMouseHoverBehavior* HoverBehavior = NewObject<UMouseHoverBehavior>();
	HoverBehavior->Initialize(this); 
	AddInputBehavior(HoverBehavior);

	UClickDragInputBehavior* ClickDragBehavior = NewObject<UClickDragInputBehavior>();
	ClickDragBehavior->Initialize(this);
	AddInputBehavior(ClickDragBehavior);

	HoverBehavior->Modifiers.RegisterModifier(1, FInputDeviceState::IsShiftKeyDown);
	HoverBehavior->Modifiers.RegisterModifier(2, FInputDeviceState::IsCtrlKeyDown);

	CreateGizmo();

	GhostPoint = NewObject<USkeletonPoint>();
}

void USkeletonEditingToolBase::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}

void USkeletonEditingToolBase::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	if (!PropertySet) return;

	if (PropertySet != Properties) return;

	UE_LOG(LogTemp, Warning, TEXT("Property modified"));
}

void USkeletonEditingToolBase::Shutdown(EToolShutdownType ShutdownType)
{
	DestroyGizmo();
}

void USkeletonEditingToolBase::AssignProperties()
{
	// Create the property set and register it with the Tool
	Properties = NewObject<USkeletonEditingToolBaseProperties>(this);
	AddToolPropertySource(Properties);

	UE_LOG(LogTemp, Warning, TEXT("Properties assigned"));

	// Register the custom detail customization
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
}

void USkeletonEditingToolBase::SelectPoint(USkeletonPoint* Point)
{
	check(Point);

	if (SelectedPoints.IsEmpty())
	{
		ShowGizmo(FTransform(Point->WorldPos));
	}

	SelectedPoints.Add(Point);
}

void USkeletonEditingToolBase::DeselectPoint(USkeletonPoint* Point)
{
	if (SelectedPoints.Contains(Point))
	{
		SelectedPoints.Remove(Point);
	}

	if (SelectedPoints.IsEmpty())
	{
		HideGizmo();
	}
}

void USkeletonEditingToolBase::DeselectAllPoints()
{
	SelectedPoints.Empty();
	HideGizmo();
}

void USkeletonEditingToolBase::DeleteSelectedPoints()
{
	for (USkeletonPoint* Point : SelectedPoints)
	{
		// Remove all lines connected to this point
		/*for (int i = 0; i < TargetSlimeMoldActor->SkeletonLines.Num(); i++)
		{
			FSkeletonLine& Line = TargetSlimeMoldActor->SkeletonLines[i];

			if (&TargetSlimeMoldActor->SkeletonPoints[Line.PointIndex1] == Point ||
				&TargetSlimeMoldActor->SkeletonPoints[Line.PointIndex2] == Point)
			{
				LinesIndicesToRemove.insert(i);
			}
		}*/

		//TargetSlimeMoldActor->SkeletonLines.RemoveAll([Point](FSkeletonLine Line) {
		//	return Line.Point1 == Point || Line.Point2 == Point;
		//	});

		for (int i = 0; i < TargetSlimeMoldActor->SkeletonLines.Num(); i++)
		{
			FSkeletonLine& Line = TargetSlimeMoldActor->SkeletonLines[i];
			if (Line.Point1 == Point || Line.Point2 == Point)
			{
				TargetSlimeMoldActor->SkeletonLines.RemoveAt(i--);
			}
		}

		TargetSlimeMoldActor->SkeletonPoints.Remove(Point);
	}
	
	DeselectAllPoints();
}

void USkeletonEditingToolBase::ConnectPoints(USkeletonPoint* Point1, USkeletonPoint* Point2)
{
	// Check if the points are already connected
	for (const FSkeletonLine& Line : TargetSlimeMoldActor->SkeletonLines)
	{
		if ((Line.Point1 == Point1 && Line.Point2 == Point2) ||
			(Line.Point1 == Point2 && Line.Point2 == Point1))
		{
			return;
		}
	}
	// Add a new line
	FSkeletonLine NewLine;
	NewLine.Point1 = Point1;
	NewLine.Point2 = Point2;
	TargetSlimeMoldActor->SkeletonLines.Add(NewLine);
}

void USkeletonEditingToolBase::DisconnectPoints(USkeletonPoint* Point1, USkeletonPoint* Point2)
{
	// Find the line to remove
	for (int i = 0; i < TargetSlimeMoldActor->SkeletonLines.Num(); i++)
	{
		FSkeletonLine& Line = TargetSlimeMoldActor->SkeletonLines[i];
		if ((Line.Point1 == Point1 && Line.Point2 == Point2) ||
			(Line.Point1 == Point2 && Line.Point2 == Point1))
		{
			TargetSlimeMoldActor->SkeletonLines.RemoveAt(i);
			return;
		}
	}
}

USkeletonPoint* USkeletonEditingToolBase::GetPointFromMousePos(const FInputDeviceRay& ClickPos)
{
	FVector HitPos;
	FInputRayHit Hit = FindRayHit(ClickPos.WorldRay, HitPos);

	if (Hit.bHit)
	{
		// Find the closest point to the hit position
		USkeletonPoint* ClosestPoint = nullptr;
		float ClosestDist = FLT_MAX;
		for (USkeletonPoint* Point : TargetSlimeMoldActor->SkeletonPoints)
		{
			float Dist = FVector::Dist(Point->WorldPos, HitPos);
			if (Dist < ClosestDist)
			{
				ClosestDist = Dist;
				ClosestPoint = Point;
			}
		}
		return ClosestPoint;
	}
	return nullptr;
}

FInputRayHit USkeletonEditingToolBase::FindRayHit(const FRay& WorldRay, FVector& HitPos)
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

void USkeletonEditingToolBase::DestroyGizmo()
{
	UInteractiveGizmoManager* const GizmoManager = GetToolManager()->GetPairedGizmoManager();
	ensure(GizmoManager);
	GizmoManager->DestroyGizmo(TransformGizmo);

	TransformGizmo = nullptr;
}

void USkeletonEditingToolBase::ShowGizmo(const FTransform& IntialTransform)
{
	PreviousGizmoTransform = IntialTransform;
	TransformGizmo->SetNewGizmoTransform(IntialTransform);
	TransformGizmo->SetVisibility(true);
}

void USkeletonEditingToolBase::HideGizmo()
{
	TransformGizmo->SetVisibility(false);
}

void USkeletonEditingToolBase::CreateGizmo()
{
	UInteractiveGizmoManager* const GizmoManager = GetToolManager()->GetPairedGizmoManager();
	ensure(GizmoManager);

	UTransformProxy* GizmoProxy = NewObject<UTransformProxy>(this);
	ensure(GizmoProxy);

	TransformGizmo = GizmoManager->Create3AxisTransformGizmo(this);
	ensure(TransformGizmo);

	TransformGizmo->SetActiveTarget(GizmoProxy, GetToolManager());

	GizmoProxy->OnTransformChanged.AddWeakLambda(this, [this](UTransformProxy*, FTransform NewTransform)
		{
			GizmoPositionDelta = NewTransform.GetLocation() - PreviousGizmoTransform.GetLocation();

			for (USkeletonPoint* Point : SelectedPoints)
			{
				Point->WorldPos += GizmoPositionDelta;
			}

			PreviousGizmoTransform = NewTransform;
		}
	);

	TransformGizmo->CurrentCoordinateSystem = EToolContextCoordinateSystem::World;
	TransformGizmo->SetVisibility(false);
}

void USkeletonEditingToolBase::Render(IToolsContextRenderAPI* RenderAPI)
{
	if (TargetSlimeMoldActor)
	{
		FPrimitiveDrawInterface* PDI = RenderAPI->GetPrimitiveDrawInterface();
		
		if (bDrawGhostPoint) 
		{
			PDI->DrawPoint(GhostPoint->WorldPos, FLinearColor(1.0f, 0.2f, 1.0f, 1.0f), Properties->DebugPointSize, SDPG_Foreground);

			if (bConnectGhostAndSelectedPoints)
			{
				for (USkeletonPoint* Point : SelectedPoints)
				{
					PDI->DrawLine(Point->WorldPos, GhostPoint->WorldPos,
						FLinearColor(1.0f, .5f, .5f, 1.0f), SDPG_Foreground, Properties->DebugLineThickness);
				}
			}
		}

		if (bDrawGhostLines)
		{
			for (USkeletonPoint* Point : SelectedPoints)
			{
				PDI->DrawLine(Point->WorldPos, GhostPoint->WorldPos,
					FLinearColor(1.0f, .5f, .5f, 1.0f), SDPG_Foreground, Properties->DebugLineThickness);
			}
		}

		// Render the skeleton with debug view
		for (const FSkeletonLine& Line : TargetSlimeMoldActor->SkeletonLines)
		{
			PDI->DrawLine(Line.Point1->WorldPos, Line.Point2->WorldPos,
				FLinearColor(0.0f, 1.0f, 1.0f, 1.0f), SDPG_Foreground, Properties->DebugLineThickness);
		}

		for (USkeletonPoint* Point : TargetSlimeMoldActor->SkeletonPoints)
		{
			FLinearColor PointColor = SelectedPoints.Contains(Point) ? FLinearColor::Red : FLinearColor::White;

			PDI->DrawPoint(Point->WorldPos, PointColor, Properties->DebugPointSize, SDPG_Foreground);
		}

	}
}

FInputRayHit USkeletonEditingToolBase::MouseHittingWorld(const FInputDeviceRay& ClickPos)
{
	FVector RayStart = ClickPos.WorldRay.Origin;
	FVector RayEnd = ClickPos.WorldRay.PointAt(99999999.f);
	FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllObjects);
	FHitResult Result;
	FInputRayHit Hit;


	if (TargetWorld->LineTraceSingleByObjectType(Result, RayStart, RayEnd, QueryParams))
	{

		Hit.bHit = true;
		Hit.HitDepth = Result.Distance;
	}
	else
	{
		Hit.bHit = false;
	}

	return Hit;
}

//void USkeletonEditingToolBase::OnClicked(const FInputDeviceRay& ClickPos)
//{
//	MouseClick(ClickPos);
//}

void USkeletonEditingToolBase::OnUpdateModifierState(int ModifierID, bool bIsOn)
{
	if (ModifierID == 1)
	{
		ShiftIsPressed = bIsOn;
	}
	if (ModifierID == 2)
	{
		CtrlIsPressed = bIsOn;
	}
}

#undef LOCTEXT_NAMESPACE
