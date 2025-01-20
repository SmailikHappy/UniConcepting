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

	UI_COMMAND(LineTool, "Linker", "Click to set the origin, then click to extend the line.", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(LineTool);

	UI_COMMAND(SimpleTool, "Show Actor Info", "Opens message box with info about a clicked actor", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(SimpleTool);

	UI_COMMAND(InteractiveTool, "Measure distance", "Measures distance between 2 points (click to set the origin, shift-click to set the end-point)", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(InteractiveTool);

	UI_COMMAND(ExampleTool, "Example Tool", "An example tool that does nothing", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(ExampleTool);

	UI_COMMAND(WorldManagingTool, "World Managing Tool", "This tool helps to manage the geometry for the slimemold as well as spawns these slime-mode sources around", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(WorldManagingTool);

	UI_COMMAND(SourceManagingTool, "Source Managing Tool", "This tool manages the slimemold sources as their visuals", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(SourceManagingTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FSlimeMoldEditorToolModeCommands::GetCommands()
{
	return FSlimeMoldEditorToolModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE