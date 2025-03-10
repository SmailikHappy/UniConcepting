// Copyright Epic Games, Inc. All Rights Reserved.

#include "PointManagerTool.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "BaseBehaviors/SingleClickBehavior.h"

#include "Subsystems/UnrealEditorSubsystem.h"

// for raycast into World
#include "CollisionQueryParams.h"
#include "Engine/World.h"

#include "LevelEditorViewport.h"

#include "SlimeMoldEditorToolFunctionLibrary.h"


// localization namespace
#define LOCTEXT_NAMESPACE "UPointManagerTool"

/*
 * Tool builder
 */

bool UPointManagerToolBuilder::CanBuildTool(const FToolBuilderState& SceneState) const
{
	return USlimeMoldEditorFuncLib::SingleSlimeMoldObjectIsSelected();
}

UInteractiveTool* UPointManagerToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UPointManagerTool* NewTool = NewObject<UPointManagerTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

/*
 * Tool logic
 */

void UPointManagerTool::Setup()
{
	TargetSlimeMoldActor = USlimeMoldEditorFuncLib::GetSingleSelectedSlimeMoldObject();
	check(TargetSlimeMoldActor);

	USkeletonEditingToolBase::Setup();

	AssignProperties();
}

void UPointManagerTool::Shutdown(EToolShutdownType ShutdownType)
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

	USkeletonEditingToolBase::Shutdown(ShutdownType);
}

void UPointManagerTool::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}

void UPointManagerTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	if (!PropertySet) return;

	if (PropertySet != Properties) return;

	Property->GetName();

	// Log the name of the modified property
	UE_LOG(LogTemp, Warning, TEXT("Property modified: %s"), *Property->GetName());

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

void UPointManagerTool::MouseUpdate(const FInputDeviceRay& DevicePos)
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

void UPointManagerTool::MousePressed()
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

void UPointManagerTool::MouseReleased()
{
	DrawDebugMouseInfo(MouseRayWhenReleased, FColor::Magenta);
}

TSet<USkeletonPoint*> UPointManagerTool::GetPointsInMouseRegion(const FInputDeviceRay& DevicePos, float RayRadiusCoefficent)
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

USkeletonPoint* UPointManagerTool::GetClosestPointToMouse(const FInputDeviceRay& DevicePos, TSet<USkeletonPoint*>& SetOfPoints)
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

void UPointManagerTool::DrawDebugMouseInfo(const FInputDeviceRay& DevicePos, FColor Color)
{
	if (!bDrawDebugMouseInfo) return;

	FVector WorldPosToDraw;
	FindRayHit(DevicePos.WorldRay, WorldPosToDraw);
	DrawDebugPoint(TargetWorld, WorldPosToDraw, 10.0f, Color, false, 0.1f);
}

USkeletonPoint* UPointManagerTool::CreatePoint(const FInputDeviceRay& ClickPos)
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

void UPointManagerTool::Msg(const FString& Msg)
{
	FText Title = LOCTEXT("ActorInfoDialogTitle", "Info");

	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Msg), Title);

	UE_LOG(LogTemp, Warning, TEXT("some text lol"));
}



#undef LOCTEXT_NAMESPACE
