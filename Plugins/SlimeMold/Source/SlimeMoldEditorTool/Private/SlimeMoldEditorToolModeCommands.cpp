// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlimeMoldEditorToolModeCommands.h"
#include "SlimeMoldEditorToolMode.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "FSlimeMoldEditorToolModeCommands"

FSlimeMoldEditorToolModeCommands::FSlimeMoldEditorToolModeCommands()
	: TCommands<FSlimeMoldEditorToolModeCommands>(
		"SlimeMoldEditorToolMode", // Context name for fast lookup
		NSLOCTEXT("SlimeMoldEditorToolMode", "SlimeMoldEditorToolModeCommands", "SlimeMoldEditor Tool Mode"), // Localized context name for displaying
		NAME_None, // Parent context name. 
		FEditorStyle::GetStyleSetName() // Icon Style Set
	)
{
}

void FSlimeMoldEditorToolModeCommands::RegisterCommands()
{
	TArray<TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);

	UI_COMMAND(InteractiveTool, "Measure distance", "Measures distance between 2 points (click to set the origin, shift-click to set the end-point)", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(InteractiveTool);

	UI_COMMAND(ExampleTool, "Example Tool", "An example tool that does nothing", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(ExampleTool);

	UI_COMMAND(PointManagerTool, "Point Manager Tool", "This tool manages points", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(PointManagerTool);

	UI_COMMAND(LineManagerTool, "Line Manager Tool", "This tool manages lines", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(LineManagerTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FSlimeMoldEditorToolModeCommands::GetCommands()
{
	return FSlimeMoldEditorToolModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE