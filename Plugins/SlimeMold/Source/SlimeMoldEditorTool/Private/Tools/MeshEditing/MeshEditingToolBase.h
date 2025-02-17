// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/ClickDragTool.h"

// SlimeMold actors
#include "SlimeMoldBase.h"

// Adding gizmo to the world
#include "BaseGizmos/TransformGizmoUtil.h"
#include "InteractiveToolObjects.h"

// Buttons in detail panel
#include "Components/Button.h"

#include "MeshEditingToolBase.generated.h"

/**
 * Tool Builder
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API UMeshEditingToolBaseBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};


/**
 * Property set
 */
UCLASS(Config = EditorPerProjectUserSettings)
class SLIMEMOLDEDITORTOOL_API UMeshEditingToolBaseProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	UMeshEditingToolBaseProperties() {}

	UPROPERTY(EditAnywhere, Category = "yikes")
	float flaot;
};



/**
 * Tool logic
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API UMeshEditingToolBase : public UInteractiveTool, public IClickDragBehaviorTarget
{
	GENERATED_BODY()

public:
	virtual void SetWorld(UWorld* World);

	/** UInteractiveTool overrides */
	virtual void Setup() override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;
	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;
	virtual void Render(IToolsContextRenderAPI* RenderAPI) override;


	/** IClickDragBehaviorTarget implementation */
	virtual FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override;
	virtual void OnClickPress(const FInputDeviceRay& PressPos) override;
	virtual void OnClickRelease(const FInputDeviceRay& ReleasePos) override {}
	virtual void OnTerminateDragSequence() override {}
	virtual void OnClickDrag(const FInputDeviceRay& DragPos) override {}

protected:
	/** Properties of the tool are stored here */
	UPROPERTY()
	TObjectPtr<UMeshEditingToolBaseProperties> Properties;

protected:

	UWorld* TargetWorld = nullptr;		// target World we will raycast into

	FInputRayHit FindRayHit(const FRay& WorldRay, FVector& HitPos);		// raycasts into World
};
