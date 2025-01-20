// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/ClickDragTool.h"

// SlimeMold actors
#include "SlimeMoldSourceActor.h"

// Adding gizmo to the world
#include "BaseGizmos/TransformGizmoUtil.h"
#include "InteractiveToolObjects.h"

#include "SlimeMoldEditorToolSourceManaging.generated.h"


/**
 * Builder for USlimeMoldEditorToolSourceManaging
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API USlimeMoldEditorToolSourceManagingBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};


/**
 * Property set for the USlimeMoldEditorToolSourceManaging
 */
UCLASS(Config = EditorPerProjectUserSettings)
class SLIMEMOLDEDITORTOOL_API USlimeMoldEditorToolSourceManagingProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	USlimeMoldEditorToolSourceManagingProperties() {}

	UPROPERTY(EditAnywhere, Category = "loooooooooooooooooooooool", meta = (ClampMin = "0.0"))
	float SomeProperty = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Slime Mold source properties", meta = (ClampMin = "0.0", ClampMax = "200.0"))
	float Radius = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Slime Mold source properties", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	int32 BranchCount = 0;
};



/**
 * 
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API USlimeMoldEditorToolSourceManaging : public UInteractiveTool, public IClickDragBehaviorTarget
{
	GENERATED_BODY()

public:
	virtual void SetWorld(UWorld* World);

	/** UInteractiveTool overrides */
	virtual void Setup() override;
	//virtual void Shutdown(EToolShutdownType ShutdownType) override;
	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;
	virtual void Render(IToolsContextRenderAPI* RenderAPI) override;


	/** IClickDragBehaviorTarget implementation */
	virtual FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override;
	virtual void OnClickPress(const FInputDeviceRay& PressPos) override;
	virtual void OnClickRelease(const FInputDeviceRay& ReleasePos) override {}
	virtual void OnTerminateDragSequence() override {}
	virtual void OnClickDrag(const FInputDeviceRay& DragPos) override {}

	/** Custom functions from customization */
	FReply AcceptSlimeMoldSource();

protected:
	/** Properties of the tool are stored here */
	UPROPERTY()
	TObjectPtr<USlimeMoldEditorToolSourceManagingProperties> Properties;

protected:

	UWorld* TargetWorld = nullptr;		// target World we will raycast into

	FInputRayHit FindRayHit(const FRay& WorldRay, FVector& HitPos);		// raycasts into World

	/** Custom setup calls*/
	void SetupGizmo();		// sets up gizmos for the tool


	UTransformProxy* TransformProxy = nullptr;
	UCombinedTransformGizmo* TransformGizmo = nullptr;
	ASlimeMoldSource* CurrentSlimeMoldSource = nullptr;
};
