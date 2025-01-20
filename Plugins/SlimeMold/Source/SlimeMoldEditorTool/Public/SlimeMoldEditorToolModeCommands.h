// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

/**
 * This class contains info about the full set of commands used in this editor mode.
 */
class FSlimeMoldEditorToolModeCommands: public TCommands<FSlimeMoldEditorToolModeCommands>
{
public:
	FSlimeMoldEditorToolModeCommands();

	virtual void RegisterCommands() override;
	static TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetCommands();


	TSharedPtr<FUICommandInfo> LineTool;
	TSharedPtr<FUICommandInfo> SimpleTool;
	TSharedPtr<FUICommandInfo> InteractiveTool;
	TSharedPtr<FUICommandInfo> ExampleTool;
	TSharedPtr<FUICommandInfo> WorldManagingTool;
	TSharedPtr<FUICommandInfo> SourceManagingTool;

protected:
	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> Commands;
};