// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"

#include "SkeletonEditingToolBase.h"


#include "PointManagerTool.generated.h"

/**
 * This class should not have any Tool Builder
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API UPointManagerToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()
 
public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override;
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};



/**
 * Tool logic
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API UPointManagerTool : public USkeletonEditingToolBase
{
	GENERATED_BODY()

public:
	virtual void SetWorld(UWorld* World);

	/** UInteractiveTool overrides */
	virtual void Setup() override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;
	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;

protected:

	/** USkeletonEditingToolBase overrides */
	//void MouseClick(const FInputDeviceRay& ClickPos);
	void MouseUpdate(const FInputDeviceRay& DevicePos);
	void MousePressed();
	void MouseReleased();


	void CreatePoint(const FInputDeviceRay& ClickPos);
	void Msg(const FString& Msg);

};
