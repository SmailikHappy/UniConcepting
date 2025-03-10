// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlimeMoldSkeletonEditingTool.h"
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


// Custom static functions
#include "SlimeMoldEditorToolFunctionLibrary.h"



// localization namespace
#define LOCTEXT_NAMESPACE "USlimeMoldSkeletonEditingTool"


/*
 * Tool builder
 */

bool USlimeMoldSkeletonEditingToolBuilder::CanBuildTool(const FToolBuilderState& SceneState) const
{
	return USlimeMoldEditorFuncLib::SingleSlimeMoldObjectIsSelected();
}

UInteractiveTool* USlimeMoldSkeletonEditingToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	USlimeMoldSkeletonEditingTool* NewTool = NewObject<USlimeMoldSkeletonEditingTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}



/*
 * Tool logic
 */

void USlimeMoldSkeletonEditingTool::Setup()
{
	TargetSlimeMoldActor = USlimeMoldEditorFuncLib::GetSingleSelectedSlimeMoldObject();
	check(TargetSlimeMoldActor);

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

	AssignProperties();
}

void USlimeMoldSkeletonEditingTool::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}

void USlimeMoldSkeletonEditingTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	if (!PropertySet) return;

	if (PropertySet != Properties) return;

	Property->GetName();

	if (Property->GetName() == "bDeletePoints")
	{
		DeleteSelectedPoints();
	}
	else if (Property->GetName() == "bDisconnectPoints")
	{
		DisconnectSelectedPoints();
	}
	else if (Property->GetName() == "bSplitLineInMid")
	{
		TArray<FSkeletonLine> SelectedLines = GetSelectedLines();

		for (FSkeletonLine LineToSplit : SelectedLines)
		{
			SplitLine(LineToSplit);
		}
	}
}

void USlimeMoldSkeletonEditingTool::Shutdown(EToolShutdownType ShutdownType)
{
	DestroyGizmo();
}

/*
 * Full mouse controls
 */
void USlimeMoldSkeletonEditingTool::MousePressed()
{
	DrawDebugMouseInfo(MouseRayWhenPressed, FColor::Green);

	// Select single point
	if (!ShiftIsPressed && !CtrlIsPressed)
	{
		DeselectAllPoints();

		TSet<USkeletonPoint*> PointsInRegion = GetPointsInMouseRegion(MouseRayWhenPressed, MouseRadiusCoefficent);

		if (!PointsInRegion.IsEmpty())
		{
			USkeletonPoint* ClosestPoint = GetClosestPointToMouse(MouseRayWhenPressed, PointsInRegion);
			SelectPoint(ClosestPoint);
		}
	}

	// Select another point
	if (!ShiftIsPressed && CtrlIsPressed)
	{
		TSet<USkeletonPoint*> PointsInRegion = GetPointsInMouseRegion(MouseRayWhenPressed, MouseRadiusCoefficent);

		if (!PointsInRegion.IsEmpty())
		{
			USkeletonPoint* ClosestPoint = GetClosestPointToMouse(MouseRayWhenPressed, PointsInRegion);
			SelectPoint(ClosestPoint);
		}
	}

	// Connect points / create and connect
	if (ShiftIsPressed && !CtrlIsPressed)
	{
		TSet<USkeletonPoint*> PointsInRegion = GetPointsInMouseRegion(MouseRayWhenPressed, MouseRadiusCoefficent);

		USkeletonPoint* JustConnectedPoint = nullptr;

		if (!PointsInRegion.IsEmpty())
		{
			USkeletonPoint* ClosestPoint = GetClosestPointToMouse(MouseRayWhenPressed, PointsInRegion);
			if (ClosestPoint)
			{
				for (USkeletonPoint* Point : SelectedPoints)
				{
					ConnectPoints(Point, ClosestPoint);
				}
			}

			JustConnectedPoint = ClosestPoint;
		}
		else
		{
			USkeletonPoint* NewPoint = CreatePoint(MouseRayWhenPressed);

			for (USkeletonPoint* Point : SelectedPoints)
			{
				ConnectPoints(Point, NewPoint);
			}

			JustConnectedPoint = NewPoint;
		}

		// Chenge selection from selected points just connected one
		if (Properties->bChangeSelectionOnPointCreate)
		{
			DeselectAllPoints();
			SelectPoint(JustConnectedPoint);
		}
	}
}

void USlimeMoldSkeletonEditingTool::MouseUpdate(const FInputDeviceRay& DevicePos)
{
	DrawDebugMouseInfo(DevicePos, MouseIsPressed ? FColor::Blue : FColor::Cyan);

	bDrawGhostPoint = false;
	bDrawGhostLines = false;

	// Multiple point selection with just mouse hovering around
	if (MouseIsPressed && !ShiftIsPressed && CtrlIsPressed)
	{
		TSet<USkeletonPoint*> PointsInRegion = GetPointsInMouseRegion(DevicePos, MouseRadiusCoefficent);
		
		for (USkeletonPoint* Point : PointsInRegion)
		{
			SelectPoint(Point);
		}
	}

	// Point visual connection / snap
	if (!MouseIsPressed && ShiftIsPressed && !CtrlIsPressed)
	{
		bDrawGhostLines = true;
		TSet<USkeletonPoint*> PointsInRegion = GetPointsInMouseRegion(DevicePos, MouseRadiusCoefficent);

		if (!PointsInRegion.IsEmpty())
		{
			USkeletonPoint* ClosestPoint = GetClosestPointToMouse(DevicePos, PointsInRegion);
			if (ClosestPoint)
			{
				// Just copy data, do not replace the GhostPoint pointer
				GhostPoint->WorldPos = ClosestPoint->WorldPos;
				GhostPoint->WorldNormal = ClosestPoint->WorldNormal;
			}
		}
		else
		{
			bDrawGhostPoint = true;
			FindRayHit(DevicePos.WorldRay, GhostPoint->WorldPos);
		}
	}
}

void USlimeMoldSkeletonEditingTool::MouseReleased()
{
	DrawDebugMouseInfo(MouseRayWhenReleased, FColor::Magenta);
}


/*
 * Helper functions
 */
void USlimeMoldSkeletonEditingTool::AssignProperties()
{
	// Create the property set and register it with the Tool
	Properties = NewObject<USlimeMoldSkeletonEditingToolProperties>(this);
	AddToolPropertySource(Properties);

	UE_LOG(LogTemp, Warning, TEXT("Properties assigned"));

	// Register the custom detail customization
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
}

FInputRayHit USlimeMoldSkeletonEditingTool::FindRayHit(const FRay& WorldRay, FVector& HitPos)
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

FInputRayHit USlimeMoldSkeletonEditingTool::MouseHittingWorld(const FInputDeviceRay& ClickPos)
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

void USlimeMoldSkeletonEditingTool::OnUpdateModifierState(int ModifierID, bool bIsOn)
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


/*
 * Skeleton related functions
 */
void USlimeMoldSkeletonEditingTool::SelectPoint(USkeletonPoint* Point)
{
	check(Point);

	if (SelectedPoints.IsEmpty())
	{
		ShowGizmo(FTransform(Point->WorldPos));
	}

	SelectedPoints.Add(Point);
}

void USlimeMoldSkeletonEditingTool::DeselectPoint(USkeletonPoint* Point)
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

void USlimeMoldSkeletonEditingTool::DeselectAllPoints()
{
	SelectedPoints.Empty();
	HideGizmo();
}

void USlimeMoldSkeletonEditingTool::DeleteSelectedPoints()
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

void USlimeMoldSkeletonEditingTool::ConnectPoints(USkeletonPoint* Point1, USkeletonPoint* Point2)
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

void USlimeMoldSkeletonEditingTool::DisconnectPoints(USkeletonPoint* Point1, USkeletonPoint* Point2)
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

void USlimeMoldSkeletonEditingTool::DisconnectSelectedPoints()
{
	// UNOPTIMIZED -> Check on speeds
	for (USkeletonPoint* Point1 : SelectedPoints)
	{
		for (USkeletonPoint* Point2 : SelectedPoints)
		{
			if (Point1 != Point2)
			{
				DisconnectPoints(Point1, Point2);
			}
		}
	}
}

TArray<FSkeletonLine> USlimeMoldSkeletonEditingTool::GetSelectedLines()
{
	TArray<FSkeletonLine> LineArray;

	for (FSkeletonLine& Line : TargetSlimeMoldActor->SkeletonLines)
	{
		if (SelectedPoints.Contains(Line.Point1) && SelectedPoints.Contains(Line.Point2))
		{
			LineArray.Add(Line);
		}
	}

	return LineArray;
}

TSet<USkeletonPoint*> USlimeMoldSkeletonEditingTool::GetPointsInMouseRegion(const FInputDeviceRay& DevicePos, float RayRadiusCoefficent)
{
	TSet<USkeletonPoint*> PointsInRegion;

	FEditorViewportClient* client = (FEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
	FVector CameraRotation = client->GetViewRotation().Vector();
	FVector CameraLocation = client->GetViewLocation();

	float dotProductK = FVector::DotProduct(CameraRotation, DevicePos.WorldRay.Direction);

	for (USkeletonPoint* Point : TargetSlimeMoldActor->SkeletonPoints)
	{
		FVector DirectionToPoint = Point->WorldPos - CameraLocation;
		DirectionToPoint.Normalize();

		float OneMinusDotProduct = 1.0f - FVector::DotProduct(DirectionToPoint, DevicePos.WorldRay.Direction);

		if (OneMinusDotProduct / (dotProductK * dotProductK * dotProductK) < RayRadiusCoefficent)
		{
			PointsInRegion.Add(Point);
		}
	}

	return PointsInRegion;
}

USkeletonPoint* USlimeMoldSkeletonEditingTool::GetClosestPointToMouse(const FInputDeviceRay& DevicePos, TSet<USkeletonPoint*>& SetOfPoints)
{
	// The closest point is the one with biggest dot product
	USkeletonPoint* ClosestPoint = nullptr;
	float MaxDotProduct = 0.0f;

	for (USkeletonPoint* Point : SetOfPoints)
	{
		FVector DirectionToPoint = Point->WorldPos - DevicePos.WorldRay.Origin;
		DirectionToPoint.Normalize();
		float DotProduct = FVector::DotProduct(DirectionToPoint, DevicePos.WorldRay.Direction);
		if (DotProduct > MaxDotProduct)
		{
			MaxDotProduct = DotProduct;
			ClosestPoint = Point;
		}
	}

	return ClosestPoint;
}

void USlimeMoldSkeletonEditingTool::SplitLine(FSkeletonLine line)
{
	// Create a new point in the middle of the line
	USkeletonPoint* NewPoint = NewObject<USkeletonPoint>(TargetSlimeMoldActor);
	NewPoint->WorldPos = (line.Point1->WorldPos + line.Point2->WorldPos) / 2.0f;
	NewPoint->WorldNormal = (line.Point1->WorldNormal + line.Point2->WorldNormal) / 2.0f;

	TargetSlimeMoldActor->SkeletonPoints.Add(NewPoint);

	// Create two new lines
	FSkeletonLine NewLine1;
	NewLine1.Point1 = line.Point1;
	NewLine1.Point2 = NewPoint;
	TargetSlimeMoldActor->SkeletonLines.Add(NewLine1);

	FSkeletonLine NewLine2;
	NewLine2.Point1 = NewPoint;
	NewLine2.Point2 = line.Point2;
	TargetSlimeMoldActor->SkeletonLines.Add(NewLine2);

	// Remove the old line
	TargetSlimeMoldActor->SkeletonLines.Remove(line);
}

void USlimeMoldSkeletonEditingTool::DrawDebugMouseInfo(const FInputDeviceRay& DevicePos, FColor Color)
{
	if (!bDrawDebugMouseInfo) return;

	FVector WorldPosToDraw;
	FindRayHit(DevicePos.WorldRay, WorldPosToDraw);
	DrawDebugPoint(TargetWorld, WorldPosToDraw, 10.0f, Color, false, 0.1f);
}

USkeletonPoint* USlimeMoldSkeletonEditingTool::CreatePoint(const FInputDeviceRay& ClickPos)
{
	FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllObjects);
	FHitResult Result;
	bool bHitWorld = TargetWorld->LineTraceSingleByObjectType(Result, ClickPos.WorldRay.Origin, ClickPos.WorldRay.PointAt(999999), QueryParams);
	if (bHitWorld)
	{
		// Add a new point
		USkeletonPoint* NewPoint = NewObject<USkeletonPoint>(TargetSlimeMoldActor);
		//NewPoint->SetFlags(NewPoint->GetFlags() | RF_Transactional);
		NewPoint->WorldPos = Result.Location;
		NewPoint->WorldNormal = Result.ImpactNormal;
		TargetSlimeMoldActor->SkeletonPoints.Add(NewPoint);

		return NewPoint;
	}

	return nullptr;
}

void USlimeMoldSkeletonEditingTool::Msg(const FString& Msg)
{
	FText Title = LOCTEXT("ActorInfoDialogTitle", "Info");

	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Msg), Title);

	UE_LOG(LogTemp, Warning, TEXT("some text lol"));
}

/*
 * Gizmo functionality
 */
void USlimeMoldSkeletonEditingTool::DestroyGizmo()
{
	UInteractiveGizmoManager* const GizmoManager = GetToolManager()->GetPairedGizmoManager();
	ensure(GizmoManager);
	GizmoManager->DestroyGizmo(TransformGizmo);

	TransformGizmo = nullptr;
}

void USlimeMoldSkeletonEditingTool::ShowGizmo(const FTransform& IntialTransform)
{
	PreviousGizmoTransform = IntialTransform;
	TransformGizmo->SetNewGizmoTransform(IntialTransform);
	TransformGizmo->SetVisibility(true);
}

void USlimeMoldSkeletonEditingTool::HideGizmo()
{
	TransformGizmo->SetVisibility(false);
}

void USlimeMoldSkeletonEditingTool::CreateGizmo()
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

/*
 * Rendering
 */
void USlimeMoldSkeletonEditingTool::Render(IToolsContextRenderAPI* RenderAPI)
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


#undef LOCTEXT_NAMESPACE
