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
}

void UPointManagerTool::MouseUpdate(const FInputDeviceRay& DevicePos)
{
	FVector WorldPosToDraw;
	FindRayHit(DevicePos.WorldRay, WorldPosToDraw);
	DrawDebugPoint(TargetWorld, WorldPosToDraw, 10.0f, MouseIsPressed ? FColor::Blue : FColor::Cyan, false, 0.1f);
}

void UPointManagerTool::MousePressed()
{/*
	FVector CameraLocation;
	FRotator CameraRotation;
	GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>()->GetLevelViewportCameraInfo(CameraLocation, CameraRotation);*/


	FEditorViewportClient* client = (FEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
	FVector CameraRotation = client->GetViewRotation().Vector();
	FVector CameraLocation = client->GetViewLocation();


	float dotProductK = FVector::DotProduct(CameraRotation, MouseRayWhenPressed.WorldRay.Direction);
	UE_LOG(LogTemp, Warning, TEXT("%f"), dotProductK);

	bool lol = false;
	for (USkeletonPoint* Point : TargetSlimeMoldActor->SkeletonPoints)
	{
		FVector DirectionToPoint = Point->WorldPos - CameraLocation;
		DirectionToPoint.Normalize();

		float OneMinusDotProduct = 1.0f - FVector::DotProduct(DirectionToPoint, MouseRayWhenPressed.WorldRay.Direction);

		if (OneMinusDotProduct / (dotProductK*dotProductK*dotProductK) < 0.002f)
		{
			lol = true;
			break;
		}
	}

	FVector WorldPosToDraw;
	FindRayHit(MouseRayWhenPressed.WorldRay, WorldPosToDraw);
	DrawDebugPoint(TargetWorld, WorldPosToDraw, 10.0f, lol ? FColor::Green : FColor::Black, false, 1.0f);
}

void UPointManagerTool::MouseReleased()
{
	FVector WorldPosToDraw;
	FindRayHit(MouseRayWhenReleased.WorldRay, WorldPosToDraw);
	DrawDebugPoint(TargetWorld, WorldPosToDraw, 10.0f, FColor::Magenta, false, 1.0f);
}

void UPointManagerTool::CreatePoint(const FInputDeviceRay& ClickPos)
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
		SelectPoint(NewPoint);
	}
}

void UPointManagerTool::Msg(const FString& Msg)
{
	FText Title = LOCTEXT("ActorInfoDialogTitle", "Info");

	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Msg), Title);

	UE_LOG(LogTemp, Warning, TEXT("some text lol"));
}

#undef LOCTEXT_NAMESPACE
