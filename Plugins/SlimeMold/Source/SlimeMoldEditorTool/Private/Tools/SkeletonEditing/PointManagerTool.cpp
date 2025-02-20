// Copyright Epic Games, Inc. All Rights Reserved.

#include "PointManagerTool.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "BaseBehaviors/SingleClickBehavior.h"

// for raycast into World
#include "CollisionQueryParams.h"
#include "Engine/World.h"


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

void UPointManagerTool::MouseClick(const FInputDeviceRay& ClickPos)
{
	if (CtrlIsPressed)
	{
		ConnectPoints(SelectedPoints.Get(FSetElementId::FromInteger(0)), SelectedPoints.Get(FSetElementId::FromInteger(SelectedPoints.Num() - 1)));
		return;
	}

	if (ShiftIsPressed)
	{
		DeleteSelectedPoints();
		return;
	}

	USkeletonPoint* ClickedPoint = GetPointFromMousePos(ClickPos);
	if (ClickedPoint)
	{
		SelectPoint(ClickedPoint);
	}
}

void UPointManagerTool::RegisterActions(FInteractiveToolActionSet& ActionSet)
{
	ActionSet.RegisterAction(
		this,
		(int32)EStandardToolActions::BaseClientDefinedActionID + 61,
		TEXT("Action1"),
		LOCTEXT("Action1Key", "Action 1 SUIN"),
		LOCTEXT("Action1Desc", "Action 1 description"),
		EModifierKey::None,
		EKeys::W,
		[this]() { Msg("Action 1 executed"); }
	);

	UE_LOG(LogTemp, Warning, TEXT("actions registered"));
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
