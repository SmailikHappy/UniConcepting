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
	USkeletonEditingToolBase::Setup();

	TargetSlimeMoldActor = USlimeMoldEditorFuncLib::GetSingleSelectedSlimeMoldObject();

	check(TargetSlimeMoldActor);

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
	FText Title = LOCTEXT("ActorInfoDialogTitle", "Actor Info");
	FText ActorInfoMsg = LOCTEXT("ActorInfoDialogMsg", "No msg");

	if (CtrlIsPressed) {
		ActorInfoMsg = LOCTEXT("ActorInfoDialogMsg", "Ctrl msg");
	}
	if (ShiftIsPressed) {
		ActorInfoMsg = LOCTEXT("ActorInfoDialogMsg", "Shift msg");
	}

	FMessageDialog::Open(EAppMsgType::Ok, ActorInfoMsg, Title);
}

#undef LOCTEXT_NAMESPACE
