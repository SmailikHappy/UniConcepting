// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"

#include "SkeletonEditingToolBase.h"


#include "LineManagerTool.generated.h"

/**
 * This class should not have any Tool Builder
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API ULineManagerToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};



/**
 * Tool logic
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API ULineManagerTool : public USkeletonEditingToolBase
{
	GENERATED_BODY()

public:
	virtual void SetWorld(UWorld* World);

	/** UInteractiveTool overrides */
	virtual void Setup() override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;
	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;

	/** USkeletonEditingToolBase overrides */
	void MouseClick(const FInputDeviceRay& ClickPos) override;
};
