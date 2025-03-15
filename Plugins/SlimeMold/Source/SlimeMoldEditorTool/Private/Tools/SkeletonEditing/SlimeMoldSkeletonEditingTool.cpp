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
	return USlimeMoldEditorFuncLib::SingleActorWithSkeletonComponentIsSelected();
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
	TargetActorComponent = USlimeMoldEditorFuncLib::GetSkeletonComponentFromSelectedActor();
	check(TargetActorComponent);

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

		TSet<int32> PointsInRegion = GetPointIDsInMouseRegion(MouseRayWhenPressed, MouseRadiusCoefficent);

		if (!PointsInRegion.IsEmpty())
		{
			int32 ClosestPointID;
			GetClosestPointToMouse(MouseRayWhenPressed, PointsInRegion, ClosestPointID);
			SelectPoint(ClosestPointID);
		}
	}

	// Select another point
	if (!ShiftIsPressed && CtrlIsPressed)
	{
		TSet<int32> PointIDsInRegion = GetPointIDsInMouseRegion(MouseRayWhenPressed, MouseRadiusCoefficent);

		// Select the closest point
		if (!PointIDsInRegion.IsEmpty())
		{
			int32 ClosestPointID;
			GetClosestPointToMouse(MouseRayWhenPressed, PointIDsInRegion, ClosestPointID);
			SelectPoint(ClosestPointID);
		}
	}

	// Connect points / create and connect
	if (ShiftIsPressed && !CtrlIsPressed)
	{
		TSet<int32> PointsInRegion = GetPointIDsInMouseRegion(MouseRayWhenPressed, MouseRadiusCoefficent);

		int32 JustConnectedPointID = -1;

		if (!PointsInRegion.IsEmpty())
		{
			int32 ClosestPointID;
			GetClosestPointToMouse(MouseRayWhenPressed, PointsInRegion, ClosestPointID);

			if (ClosestPointID != -1)
			{
				for (int32 PointID : SelectedPointIDs)
				{
					ConnectPoints(PointID, ClosestPointID);
				}
			}

			JustConnectedPointID = ClosestPointID;
		}
		else
		{
			int32 NewPointID;
			CreatePoint(MouseRayWhenPressed, NewPointID);

			for (int32 PointID : SelectedPointIDs)
			{
				ConnectPoints(PointID, NewPointID);
			}

			JustConnectedPointID = NewPointID;
		}

		// Chenge selection from selected points just connected one
		if (Properties->bChangeSelectionOnPointCreate)
		{
			DeselectAllPoints();

			if (JustConnectedPointID != -1) {
				SelectPoint(JustConnectedPointID);
			}
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
		TSet<int32> PointIDsInRegion = GetPointIDsInMouseRegion(DevicePos, MouseRadiusCoefficent);
		
		for (int32 PointID : PointIDsInRegion)
		{
			SelectPoint(PointID);
		}
	}

	// Point visual connection / snap
	if (!MouseIsPressed && ShiftIsPressed && !CtrlIsPressed)
	{
		bDrawGhostLines = true;
		TSet<int32> PointsInRegion = GetPointIDsInMouseRegion(DevicePos, MouseRadiusCoefficent);

		if (!PointsInRegion.IsEmpty())
		{
			int32 ClosestPointID;
			FSkeletonPoint& ClosestPoint = GetClosestPointToMouse(DevicePos, PointsInRegion, ClosestPointID);
			if (ClosestPointID != -1)
			{
				GhostPoint = ClosestPoint;
			}
		}
		else
		{
			bDrawGhostPoint = true;
			FindRayHit(DevicePos.WorldRay, GhostPoint.WorldPos);
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
void USlimeMoldSkeletonEditingTool::SelectPoint(int32 PointID)
{
	if (SelectedPointIDs.IsEmpty())
	{
		ShowGizmo(FTransform(TargetActorComponent->SkeletonPoints[PointID].WorldPos));
	}

	SelectedPointIDs.Add(PointID);
}

void USlimeMoldSkeletonEditingTool::DeselectPoint(int32 PointID)
{
	if (SelectedPointIDs.Contains(PointID))
	{
		SelectedPointIDs.Remove(PointID);
	}

	if (SelectedPointIDs.IsEmpty())
	{
		HideGizmo();
	}
}

void USlimeMoldSkeletonEditingTool::DeselectAllPoints()
{
	SelectedPointIDs.Empty();
	HideGizmo();
}

void USlimeMoldSkeletonEditingTool::DeleteSelectedPoints()
{
	// Remove connected lines to these points first
	for (int32 PointID : SelectedPointIDs)
	{
		for (int i = 0; i < TargetActorComponent->SkeletonLines.Num(); i++)
		{
			FSkeletonLine& Line = TargetActorComponent->SkeletonLines[i];
			if (Line.Point1ID == PointID || Line.Point2ID == PointID)
			{
				TargetActorComponent->SkeletonLines.RemoveAt(i--);
			}
		}
	}

	// Reconnect lines to right IDs
	TArray<int32> SelectedPointIDsArray = SelectedPointIDs.Array();
	SelectedPointIDsArray.Sort();

	DeselectAllPoints();

	while (!SelectedPointIDsArray.IsEmpty())
	{
		int32 PointIDToRemove = SelectedPointIDsArray[0];

		TargetActorComponent->SkeletonPoints.RemoveAt(PointIDToRemove);

		// Lower the indices of the points in the lines
		for (FSkeletonLine& Line : TargetActorComponent->SkeletonLines)
		{
			if (Line.Point1ID > PointIDToRemove)
			{
				Line.Point1ID--;
			}
			if (Line.Point2ID > PointIDToRemove)
			{
				Line.Point2ID--;
			}
		}

		// Lower the indices of the selected points
		for (int32& PointID : SelectedPointIDsArray)
		{
			PointID--;
		}

		SelectedPointIDsArray.RemoveAt(0);
	}
}

void USlimeMoldSkeletonEditingTool::ConnectPoints(int32 Point1ID, int32 Point2ID)
{
	// Add a new line
	FSkeletonLine NewLine;
	NewLine.Point1ID = Point1ID;
	NewLine.Point2ID = Point2ID;
	// Check if the points are already connected
	for (const FSkeletonLine& Line : TargetActorComponent->SkeletonLines)
	{
		if (NewLine == Line)
		{
			return;
		}
	}
	TargetActorComponent->SkeletonLines.Add(NewLine);
}

void USlimeMoldSkeletonEditingTool::DisconnectPoints(int32 Point1ID, int32 Point2ID)
{
	// Find the line to remove
	FSkeletonLine LineToRemove(Point1ID, Point2ID);
	for (int i = 0; i < TargetActorComponent->SkeletonLines.Num(); i++)
	{
		FSkeletonLine& Line = TargetActorComponent->SkeletonLines[i];
		if (LineToRemove == Line)
		{
			TargetActorComponent->SkeletonLines.RemoveAt(i);
			return;
		}
	}
}

void USlimeMoldSkeletonEditingTool::DisconnectSelectedPoints()
{
	// UNOPTIMIZED -> Check on speeds
	for (int32 Point1ID : SelectedPointIDs)
	{
		for (int32 Point2ID : SelectedPointIDs)
		{
			if (Point1ID != Point2ID)
			{
				DisconnectPoints(Point1ID, Point2ID);
			}
		}
	}
}

TArray<FSkeletonLine> USlimeMoldSkeletonEditingTool::GetSelectedLines()
{
	TArray<FSkeletonLine> LineArray;

	for (FSkeletonLine& Line : TargetActorComponent->SkeletonLines)
	{
		if (SelectedPointIDs.Contains(Line.Point1ID) && SelectedPointIDs.Contains(Line.Point2ID))
		{
			LineArray.Add(Line);
		}
	}

	return LineArray;
}

TSet<int32> USlimeMoldSkeletonEditingTool::GetPointIDsInMouseRegion(const FInputDeviceRay& DevicePos, float RayRadiusCoefficent)
{
	TSet<int32> PointsInRegion;

	FEditorViewportClient* client = (FEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
	FVector CameraRotation = client->GetViewRotation().Vector();
	FVector CameraLocation = client->GetViewLocation();

	float dotProductK = FVector::DotProduct(CameraRotation, DevicePos.WorldRay.Direction);

	for (int32 PointID = 0; PointID < TargetActorComponent->SkeletonPoints.Num(); PointID++)
	{
		FSkeletonPoint& Point = TargetActorComponent->SkeletonPoints[PointID];

		FVector DirectionToPoint = Point.WorldPos - CameraLocation;
		DirectionToPoint.Normalize();

		float OneMinusDotProduct = 1.0f - FVector::DotProduct(DirectionToPoint, DevicePos.WorldRay.Direction);

		if (OneMinusDotProduct / (dotProductK * dotProductK * dotProductK) < RayRadiusCoefficent)
		{
			PointsInRegion.Add(PointID);
		}
	}

	return PointsInRegion;
}

FSkeletonPoint& USlimeMoldSkeletonEditingTool::GetClosestPointToMouse(const FInputDeviceRay& DevicePos, const TSet<int32>& SetOfPoints, int32& ClosestPointID)
{
	// The closest point is the one with biggest dot product
	ClosestPointID = -1;
	float MaxDotProduct = 0.0f;

	for (int32 PointID : SetOfPoints)
	{
		FSkeletonPoint& Point = TargetActorComponent->SkeletonPoints[PointID];
		FVector DirectionToPoint = Point.WorldPos - DevicePos.WorldRay.Origin;
		DirectionToPoint.Normalize();
		float DotProduct = FVector::DotProduct(DirectionToPoint, DevicePos.WorldRay.Direction);
		if (DotProduct > MaxDotProduct)
		{
			MaxDotProduct = DotProduct;
			ClosestPointID = PointID;
		}
	}

	return TargetActorComponent->SkeletonPoints[ClosestPointID];
}

void USlimeMoldSkeletonEditingTool::SplitLine(const FSkeletonLine& Line)
{
	// Create a new point in the middle of the line
	FSkeletonPoint NewPoint;
	FSkeletonPoint& LinePoint1 = TargetActorComponent->SkeletonPoints[Line.Point1ID];
	FSkeletonPoint& LinePoint2 = TargetActorComponent->SkeletonPoints[Line.Point2ID];

	NewPoint.WorldPos = (LinePoint1.WorldPos + LinePoint2.WorldPos) / 2.0f;
	NewPoint.WorldNormal = (LinePoint1.WorldNormal + LinePoint2.WorldNormal) / 2.0f;

	int32 NewPointID = TargetActorComponent->SkeletonPoints.Add(NewPoint);

	// Create two new lines
	FSkeletonLine NewLine1;
	NewLine1.Point1ID = Line.Point1ID;
	NewLine1.Point2ID = NewPointID;
	TargetActorComponent->SkeletonLines.Add(NewLine1);

	FSkeletonLine NewLine2;
	NewLine2.Point1ID = NewPointID;
	NewLine2.Point2ID = Line.Point2ID;
	TargetActorComponent->SkeletonLines.Add(NewLine2);

	// Remove the old line
	TargetActorComponent->SkeletonLines.Remove(Line);
}

void USlimeMoldSkeletonEditingTool::DrawDebugMouseInfo(const FInputDeviceRay& DevicePos, FColor Color)
{
	if (!bDrawDebugMouseInfo) return;

	FVector WorldPosToDraw;
	FindRayHit(DevicePos.WorldRay, WorldPosToDraw);
	DrawDebugPoint(TargetWorld, WorldPosToDraw, 10.0f, Color, false, 0.1f);
}

FSkeletonPoint& USlimeMoldSkeletonEditingTool::CreatePoint(const FInputDeviceRay& ClickPos, int32& NewPointID)
{
	FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllObjects);
	FHitResult Result;
	bool bHitWorld = TargetWorld->LineTraceSingleByObjectType(Result, ClickPos.WorldRay.Origin, ClickPos.WorldRay.PointAt(999999), QueryParams);
	FSkeletonPoint NewPoint;
	NewPointID = -1;

	if (bHitWorld)
	{
		// Add a new point
		NewPoint.WorldPos = Result.Location;
		NewPoint.WorldNormal = Result.ImpactNormal;

		NewPointID = TargetActorComponent->SkeletonPoints.Add(NewPoint);
	}

	return TargetActorComponent->SkeletonPoints[NewPointID];
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

			for (int32 PointID : SelectedPointIDs)
			{
				FSkeletonPoint& Point = TargetActorComponent->SkeletonPoints[PointID];
				Point.WorldPos += GizmoPositionDelta;
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
	if (TargetActorComponent)
	{
		FPrimitiveDrawInterface* PDI = RenderAPI->GetPrimitiveDrawInterface();
		
		if (bDrawGhostPoint) 
		{
			PDI->DrawPoint(GhostPoint.WorldPos, FLinearColor(1.0f, 0.2f, 1.0f, 1.0f), Properties->DebugPointSize, SDPG_Foreground);

			if (bConnectGhostAndSelectedPoints)
			{
				for (int32 PointID : SelectedPointIDs)
				{
					FSkeletonPoint& Point = TargetActorComponent->SkeletonPoints[PointID];
					PDI->DrawLine(Point.WorldPos, GhostPoint.WorldPos,
						FLinearColor(1.0f, .5f, .5f, 1.0f), SDPG_Foreground, Properties->DebugLineThickness);
				}
			}
		}

		if (bDrawGhostLines)
		{
			for (int32 PointID : SelectedPointIDs)
			{
				FSkeletonPoint& Point = TargetActorComponent->SkeletonPoints[PointID];
				PDI->DrawLine(Point.WorldPos, GhostPoint.WorldPos,
					FLinearColor(1.0f, .5f, .5f, 1.0f), SDPG_Foreground, Properties->DebugLineThickness);
			}
		}

		// Render the skeleton with debug view
		for (const FSkeletonLine& Line : TargetActorComponent->SkeletonLines)
		{
			FVector Point1Pos = TargetActorComponent->SkeletonPoints[Line.Point1ID].WorldPos;
			FVector Point2Pos = TargetActorComponent->SkeletonPoints[Line.Point2ID].WorldPos;

			PDI->DrawLine(Point1Pos, Point2Pos,
				FLinearColor(0.0f, 1.0f, 1.0f, 1.0f), SDPG_Foreground, Properties->DebugLineThickness);
		}

		for (int32 i = 0; i < TargetActorComponent->SkeletonPoints.Num(); i++)
		{
			FLinearColor PointColor = SelectedPointIDs.Contains(i) ? FLinearColor::Red : FLinearColor::White;

			FSkeletonPoint& Point = TargetActorComponent->SkeletonPoints[i];
			PDI->DrawPoint(Point.WorldPos, PointColor, Properties->DebugPointSize, SDPG_Foreground);
		}

	}
}


#undef LOCTEXT_NAMESPACE
