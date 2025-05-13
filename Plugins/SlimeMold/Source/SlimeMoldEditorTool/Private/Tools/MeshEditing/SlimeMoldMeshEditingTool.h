// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/ClickDragTool.h"

// SlimeMold data structures
#include "SlimeMoldSkeletonComponent.h"
#include "Structs.h"

// Adding gizmo to the world
#include "BaseGizmos/TransformGizmoUtil.h"

// Custom static functions
#include "SlimeMoldEditorToolFunctionLibrary.h"


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
 * Base class for mesh properties, has to be instantiated by the user, 
 * The object of this class (with a lot of variables) will be passed to "generate mesh" event
 * So that the user can access those variables from a blueprint
 */
UCLASS(Transient, Blueprintable)
class SLIMEMOLDEDITORTOOL_API USlimeMoldMeshPropertyBase : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	USlimeMoldMeshPropertyBase() {}

	// Function to be called from the child property class itself, so the event is called from the skeleton component
	UFUNCTION(BlueprintCallable, Category = "Button")
	void ButtonPressCall(const FString& Key);

	// Ideally, we should not have any access to the tool object in the properties,
	// but we need to call a function there, that calls a function in the component that is being edited
	UPROPERTY()
	USlimeMoldMeshEditingTool* ToolObject = nullptr;
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


	// Class of custom properties that user defines for itself
	// The instance of this class will be passed via "ButtonPress" to the component being edited
	UPROPERTY(EditAnywhere, Category = "Default")
	TSubclassOf<USlimeMoldMeshPropertyBase> MeshPropertyClass;
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
	void Setup() override;
	void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;
	void Shutdown(EToolShutdownType ShutdownType) override;
	void OnTick(float DeltaTime) override;


	/** IClickDragBehaviorTarget implementation */
	FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override
		{ return USlimeMoldEditorFuncLib::FindRayHit(TargetWorld, PressPos.WorldRay); };
	void OnClickPress(const FInputDeviceRay& PressPos) override;
	void OnClickRelease(const FInputDeviceRay& ReleasePos) override {}
	void OnTerminateDragSequence() override {}
	void OnClickDrag(const FInputDeviceRay& DragPos) override {}


	// Function for custom button press events
	UFUNCTION()
	void ButtonPressEvent(const FString& ButtonKey);

protected:

	/** Tool properties */
	UPROPERTY()
	TObjectPtr<USlimeMoldMeshEditingToolProperties> ToolProperties;

	/** Mesh properties */
	UPROPERTY()
	TObjectPtr<USlimeMoldMeshPropertyBase> MeshProperties;

protected:

	UPROPERTY()
	UWorld* TargetWorld = nullptr;		// target World we will raycast into

	UPROPERTY()
	USlimeMoldSkeletonComponent* TargetActorComponent = nullptr;
};
