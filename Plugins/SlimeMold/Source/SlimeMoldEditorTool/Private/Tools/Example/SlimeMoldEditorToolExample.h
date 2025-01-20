// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/ClickDragTool.h"
#include "SlimeMoldActor.h"
#include "SlimeMoldEditorToolExample.generated.h"


/**
 * Builder for USlimeMoldEditorToolExample
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API USlimeMoldEditorToolExampleBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};


/**
 * Property set for the USlimeMoldEditorToolExample
 */
UCLASS(Config = EditorPerProjectUserSettings)
class SLIMEMOLDEDITORTOOL_API USlimeMoldEditorToolExampleProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	USlimeMoldEditorToolExampleProperties() {}

	UPROPERTY(EditAnywhere, Category = "loooooooooooooooooooooool", meta = (ClampMin = "0.0"))
	float SomeProperty = 300.0f;
};



/**
 * USlimeMoldEditorToolExample is an example Tool that allows the user to measure the
 * distance between two points. The first point is set by click-dragging the mouse, and
 * the second point is set by shift-click-dragging the mouse.
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API USlimeMoldEditorToolExample : public UInteractiveTool, public IClickDragBehaviorTarget
{
	GENERATED_BODY()

public:
	virtual void SetWorld(UWorld* World);

	/** UInteractiveTool overrides */
	virtual void Setup() override;
	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override {}
	

	/** IClickDragBehaviorTarget implementation */
	virtual FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override;
	virtual void OnClickPress(const FInputDeviceRay& PressPos) override {}
	virtual void OnClickRelease(const FInputDeviceRay& ReleasePos) override {}
	virtual void OnTerminateDragSequence() override {}
	virtual void OnClickDrag(const FInputDeviceRay& DragPos) override {}


protected:
	/** Properties of the tool are stored here */
	UPROPERTY()
	TObjectPtr<USlimeMoldEditorToolExampleProperties> Properties;


protected:


	UWorld* TargetWorld = nullptr;		// target World we will raycast into

	FInputRayHit FindRayHit(const FRay& WorldRay, FVector& HitPos);		// raycasts into World
};
