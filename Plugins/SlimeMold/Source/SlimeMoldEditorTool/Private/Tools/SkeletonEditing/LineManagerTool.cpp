// Copyright Epic Games, Inc. All Rights Reserved.

#include "LineManagerTool.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "BaseBehaviors/ClickDragBehavior.h"

// for raycast into World
#include "CollisionQueryParams.h"
#include "Engine/World.h"



// localization namespace
#define LOCTEXT_NAMESPACE "ULineManagerTool"

/*
 * Tool builder
 */

UInteractiveTool* ULineManagerToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	ULineManagerTool* NewTool = NewObject<ULineManagerTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

/*
 * Tool logic
 */

void ULineManagerTool::Setup()
{
	//USkeletonEditingToolBase::Setup();
}

void ULineManagerTool::Shutdown(EToolShutdownType ShutdownType)
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

void ULineManagerTool::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}

void ULineManagerTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	if (!PropertySet) return;

	if (PropertySet != Properties) return;
}

void ULineManagerTool::MouseClick(const FInputDeviceRay& ClickPos)
{
	FText ActorInfoMsg = LOCTEXT("ActorInfoDialogMsg", "Line msg");

	FText Title = LOCTEXT("ActorInfoDialogTitle", "Actor Info");
	// JAH TODO: consider if we can highlight the actor prior to opening the dialog box or make it non-modal
	FMessageDialog::Open(EAppMsgType::Ok, ActorInfoMsg, Title);
}

#undef LOCTEXT_NAMESPACE
