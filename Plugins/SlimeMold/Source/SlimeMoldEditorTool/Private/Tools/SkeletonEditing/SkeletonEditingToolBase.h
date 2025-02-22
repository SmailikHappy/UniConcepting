// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/ClickDragTool.h"
#include "BaseTools/SingleClickTool.h"

// SlimeMold actors
#include "SlimeMoldBase.h"
#include "Structs.h"

// Adding gizmo to the world
#include "BaseGizmos/TransformGizmoUtil.h"
#include "InteractiveToolObjects.h"

// Buttons in detail panel
#include "Components/Button.h"

#include "SkeletonEditingToolBase.generated.h"

/**
 * This class should not have any Tool Builder
 */


/**
 * Property set for every child tool
 */
UCLASS(Config = EditorPerProjectUserSettings)
class SLIMEMOLDEDITORTOOL_API USkeletonEditingToolBaseProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	USkeletonEditingToolBaseProperties() {}

	UPROPERTY(EditAnywhere, Category = "yikyyy")
	int into;


	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	float DebugLineThickness = 2.0f;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	float DebugPointSize = 10.0f;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	float SelectionMaxRadiusThreshold;
};



/**
 * Tool logic
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API USkeletonEditingToolBase : public UInteractiveTool, public IHoverBehaviorTarget, public IClickDragBehaviorTarget
{
	GENERATED_BODY()

public:
	virtual void SetWorld(UWorld* World);

	/** UInteractiveTool overrides */
	virtual void Setup() override;
	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;
	virtual void Render(IToolsContextRenderAPI* RenderAPI) override;


	/** IClickBehaviorTarget implementation */
	//virtual FInputRayHit IsHitByClick(const FInputDeviceRay& ClickPos) override;
	//virtual void OnClicked(const FInputDeviceRay& ClickPos) override;

	/** IModifierToggleBehaviorTarget implementation */
	void OnUpdateModifierState(int ModifierID, bool bIsOn) override; 

	/** IHoverBehaviorTarget implementation */
	FInputRayHit BeginHoverSequenceHitTest(const FInputDeviceRay& PressPos) override { return MouseHittingWorld(PressPos); };
	void OnBeginHover(const FInputDeviceRay& ClickPos) override {};
	void OnEndHover() override {};
	bool OnUpdateHover(const FInputDeviceRay& DevicePos) override { MouseUpdate(DevicePos); return true; };

	/** IClickDragBehaviorTarget implementation */
	FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override { return MouseHittingWorld(PressPos); };
	void OnClickPress(const FInputDeviceRay& PressPos) override { MouseRayWhenPressed = PressPos; MouseIsPressed = true; MousePressed(); };
	void OnClickDrag(const FInputDeviceRay& DragPos) override { MouseUpdate(DragPos); };
	void OnClickRelease(const FInputDeviceRay& ReleasePos) override { MouseRayWhenReleased = ReleasePos; MouseIsPressed = false; MouseReleased(); };
	void OnTerminateDragSequence() override {};

protected:
	/** Properties of the tool are stored here */
	UPROPERTY()
	TObjectPtr<USkeletonEditingToolBaseProperties> Properties;

	/** Virtual functions for child classes */
	virtual void MouseUpdate(const FInputDeviceRay& DevicePos) {};
	virtual void MousePressed() {};
	virtual void MouseReleased() {};

	
	/** Protected functions available in base */
	void AssignProperties();
	void SelectPoint(USkeletonPoint* Point);
	void DeselectPoint(USkeletonPoint* Point);
	void DeselectAllPoints();
	void DeleteSelectedPoints();
	void ConnectPoints(USkeletonPoint* Point1, USkeletonPoint* Point2);
	void DisconnectPoints(USkeletonPoint* Point1, USkeletonPoint* Point2);
	USkeletonPoint* GetPointFromMousePos(const FInputDeviceRay& ClickPos);

	FInputRayHit FindRayHit(const FRay& WorldRay, FVector& HitPos);

private:
	/** Utility functions */
	FInputRayHit MouseHittingWorld(const FInputDeviceRay& ClickPos);
	void ShowGizmo(const FTransform& IntialTransform);
	void HideGizmo();
	void CreateGizmo();
	void DestroyGizmo();
	FTransform PreviousGizmoTransform = FTransform::Identity;
	UCombinedTransformGizmo* TransformGizmo = nullptr;
	FVector GizmoPositionDelta = FVector::ZeroVector;

protected:

	bool ShiftIsPressed = false;
	bool CtrlIsPressed = false;
	bool MouseIsPressed = false;
	FInputDeviceRay MouseRayWhenPressed;
	FInputDeviceRay MouseRayWhenReleased;

	UWorld* TargetWorld = nullptr;
	ASlimeMoldBase* TargetSlimeMoldActor = nullptr;
	TSet<USkeletonPoint*> SelectedPoints;		// set ensures no duplicates
};
