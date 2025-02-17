// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlimeMoldEditorToolMode.h"
#include "SlimeMoldEditorToolModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "SlimeMoldEditorToolModeCommands.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/SlimeMoldEditorToolSimpleTool.h"
#include "Tools/SlimeMoldEditorToolInteractiveTool.h"
#include "Tools/Example/SlimeMoldEditorToolExample.h"
#include "Tools/MeshEditing/MeshEditingToolBase.h"
//#include "Tools/SkeletonEditing/SkeletonEditingToolBase.h"
#include "Tools/SkeletonEditing/PointManagerTool.h"
#include "Tools/SkeletonEditing/LineManagerTool.h"

// step 2: register a ToolBuilder in FSlimeMoldEditorToolEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "SlimeMoldEditorToolEditorMode"

const FEditorModeID USlimeMoldEditorToolEditorMode::EM_SlimeMoldEditorToolEditorModeId = TEXT("EM_SlimeMoldEditorToolEditorMode");

FString USlimeMoldEditorToolEditorMode::InteractiveToolName = TEXT("SlimeMoldEditorTool_MeasureDistanceTool");
FString USlimeMoldEditorToolEditorMode::ExampleToolName = TEXT("SlimeMoldEditorTool_ExampleTool");
FString USlimeMoldEditorToolEditorMode::PointManagerToolName = TEXT("SlimeMoldEditorTool_PointManagerTool");
FString USlimeMoldEditorToolEditorMode::LineManagerToolName = TEXT("SlimeMoldEditorTool_LineManagerTool");


USlimeMoldEditorToolEditorMode::USlimeMoldEditorToolEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(USlimeMoldEditorToolEditorMode::EM_SlimeMoldEditorToolEditorModeId,
		LOCTEXT("ModeName", "SlimeMoldEditorTool"),
		FSlateIcon(),
		true);
}


USlimeMoldEditorToolEditorMode::~USlimeMoldEditorToolEditorMode()
{
}


void USlimeMoldEditorToolEditorMode::ActorSelectionChangeNotify()
{
}

void USlimeMoldEditorToolEditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FSlimeMoldEditorToolModeCommands& SampleToolCommands = FSlimeMoldEditorToolModeCommands::Get();

	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<USlimeMoldEditorToolInteractiveToolBuilder>(this));
	RegisterTool(SampleToolCommands.ExampleTool, ExampleToolName, NewObject<USlimeMoldEditorToolExampleBuilder>(this));
	RegisterTool(SampleToolCommands.PointManagerTool, PointManagerToolName, NewObject<UPointManagerToolBuilder>(this));
	RegisterTool(SampleToolCommands.LineManagerTool, LineManagerToolName, NewObject<ULineManagerToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, ExampleToolName);
}

void USlimeMoldEditorToolEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FSlimeMoldEditorToolEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> USlimeMoldEditorToolEditorMode::GetModeCommands() const
{
	return FSlimeMoldEditorToolModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
