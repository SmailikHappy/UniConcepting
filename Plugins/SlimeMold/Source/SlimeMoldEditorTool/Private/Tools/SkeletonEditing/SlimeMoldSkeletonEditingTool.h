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
#include "InteractiveToolBuilder.h"



#include "SlimeMoldSkeletonEditingTool.generated.h"

/**
 * This is a Tool Builder for our skeleton editor
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API USlimeMoldSkeletonEditingToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override;
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};


/**
 * Property set for every child tool
 */
UCLASS(Config = EditorPerProjectUserSettings)
class SLIMEMOLDEDITORTOOL_API USlimeMoldSkeletonEditingToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	USlimeMoldSkeletonEditingToolProperties() {}

	UPROPERTY(EditAnywhere, Category = "yikyyy")
	int into;


	/** Debug draw lines */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	float DebugLineThickness = 2.0f;

	/** Debug draw points */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	float DebugPointSize = 10.0f;

	/** Mouse 'radius' point detection threshlod */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	float SelectionMaxRadiusThreshold = 1.0f;

	/** Variable listener TO REDO */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool bDeletePoints;

	/** Variable listener TO REDO*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool bDisconnectPoints;

	/** Variable listener TO REDO */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool bSplitLineInMid;

	/** Decides whether we shall change the selection to just created point (deselect all the points that were selected THEN select only the point that was just created) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool bChangeSelectionOnPointCreate = true;
};



/**
 * Tool logic
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API USlimeMoldSkeletonEditingTool : public UInteractiveTool, public IHoverBehaviorTarget, public IClickDragBehaviorTarget
{
	GENERATED_BODY()

public:
	virtual void SetWorld(UWorld* World);

	/** UInteractiveTool overrides */
	void Setup() override;
	void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;
	void Shutdown(EToolShutdownType ShutdownType) override;
	void Render(IToolsContextRenderAPI* RenderAPI) override;


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
	TObjectPtr<USlimeMoldSkeletonEditingToolProperties> Properties;

	/** Mouse controls */
	void MousePressed();
	void MouseUpdate(const FInputDeviceRay& DevicePos);
	void MouseReleased();

	/** Debug draw mouse */
	void DrawDebugMouseInfo(const FInputDeviceRay& DevicePos, FColor Color);
	
	/** Skeleton managing functions */
	void AssignProperties();
	void SelectPoint(USkeletonPoint* Point);
	void DeselectPoint(USkeletonPoint* Point);
	void DeselectAllPoints();
	void DeleteSelectedPoints();
	void ConnectPoints(USkeletonPoint* Point1, USkeletonPoint* Point2);
	void DisconnectPoints(USkeletonPoint* Point1, USkeletonPoint* Point2);
	void DisconnectSelectedPoints();
	void SplitLine(FSkeletonLine line);



	const float MouseRadiusCoefficent = 0.002f;
	bool bDrawDebugMouseInfo = false;


	/** Helper functions */
	TArray<FSkeletonLine> GetSelectedLines();
	TSet<USkeletonPoint*> GetPointsInMouseRegion(const FInputDeviceRay& DevicePos, float RayRadiusCoefficent);
	USkeletonPoint* GetClosestPointToMouse(const FInputDeviceRay& DevicePos, TSet<USkeletonPoint*>& SetOfPoints);
	FInputRayHit FindRayHit(const FRay& WorldRay, FVector& HitPos);

	USkeletonPoint* CreatePoint(const FInputDeviceRay& ClickPos);
	void Msg(const FString& Msg);

private:
	/** Utility functions */
	FInputRayHit MouseHittingWorld(const FInputDeviceRay& ClickPos);
	
	/** Gizmo functionality */
	void ShowGizmo(const FTransform& IntialTransform);
	void HideGizmo();
	void CreateGizmo();
	void DestroyGizmo();
	FTransform PreviousGizmoTransform = FTransform::Identity;
	UCombinedTransformGizmo* TransformGizmo = nullptr;
	FVector GizmoPositionDelta = FVector::ZeroVector;

protected:

	// Variable updates only when the mouse is not pressed
	bool ShiftIsPressed = false;
	// Variable updates only when the mouse is not pressed
	bool CtrlIsPressed = false;

	bool MouseIsPressed = false;
	

	FInputDeviceRay MouseRayWhenPressed;
	FInputDeviceRay MouseRayWhenReleased;

	UWorld* TargetWorld = nullptr;
	ASlimeMoldBase* TargetSlimeMoldActor = nullptr;
	TSet<USkeletonPoint*> SelectedPoints;		// set ensures no duplicates
	 
	bool bDrawGhostPoint = false;
	bool bDrawGhostLines = false;
	USkeletonPoint* GhostPoint;		// A ghost point that is not yet placed but is being shown where it could be placed OR
									// A ghost connection with selected and ghost-Point, shown where new Lines might appear
	bool bConnectGhostAndSelectedPoints = true;
};
