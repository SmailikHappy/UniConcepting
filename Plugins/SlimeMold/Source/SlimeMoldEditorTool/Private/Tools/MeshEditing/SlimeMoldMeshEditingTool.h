// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/ClickDragTool.h"

// SlimeMold actors
#include "SlimeMoldBase.h"
#include "Structs.h"

// Adding gizmo to the world
#include "BaseGizmos/TransformGizmoUtil.h"
#include "InteractiveToolObjects.h"

// Buttons in detail panel
#include "Components/Button.h"

#include "SlimeMoldMeshEditingTool.generated.h"

/**
 * Tool Builder
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API USlimeMoldMeshEditingToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override;
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};


/**
 * Property sets
 */
UCLASS(Config = EditorPerProjectUserSettings)
class SLIMEMOLDEDITORTOOL_API USlimeMoldMeshEditingToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	USlimeMoldMeshEditingToolProperties() {}

	UPROPERTY(EditAnywhere, Category = "yikes")
	float flaot;

	UPROPERTY(EditAnywhere, Category = "Default")
	bool bGenerateMesh;

	// Class of properties that will be passed via function to generate mesh
	UPROPERTY(EditAnywhere, Category = "Default")
	TSubclassOf<USlimeMoldMeshPropertyBase> PropertyClass;
};


/**
 * Base class for properties, to be instantiated in the main Tool porperties, cause the object of this class (variables) will be passed to "generate mesh function"
 */
UCLASS(Transient, Blueprintable)
class SLIMEMOLDEDITORTOOL_API USlimeMoldMeshPropertyBase : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	USlimeMoldMeshPropertyBase() {}
};





/**
 * Tool logic
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API USlimeMoldMeshEditingTool : public UInteractiveTool, public IClickDragBehaviorTarget
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
	TObjectPtr<USlimeMoldMeshEditingToolProperties> ToolProperties;

	UPROPERTY()
	TObjectPtr<USlimeMoldMeshPropertyBase> MeshProperties;

protected:

	UWorld* TargetWorld = nullptr;		// target World we will raycast into
	ASlimeMoldBase* TargetSlimeMoldActor = nullptr;

	FInputRayHit FindRayHit(const FRay& WorldRay, FVector& HitPos);		// raycasts into World
};
