// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <set>
#include <vector>
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
class SLIMEMOLDEDITORTOOL_API USkeletonEditingToolBase : public UInteractiveTool, public IClickBehaviorTarget
{
	GENERATED_BODY()

public:
	virtual void SetWorld(UWorld* World);

	/** UInteractiveTool overrides */
	virtual void Setup() override;
	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;
	virtual void Render(IToolsContextRenderAPI* RenderAPI) override;


	/** IClickBehaviorTarget implementation */
	virtual FInputRayHit IsHitByClick(const FInputDeviceRay& ClickPos) override;
	virtual void OnClicked(const FInputDeviceRay& ClickPos) override;
	void OnUpdateModifierState(int ModifierID, bool bIsOn) override; 

protected:
	/** Properties of the tool are stored here */
	UPROPERTY()
	TObjectPtr<USkeletonEditingToolBaseProperties> Properties;

	/** Virtual functions for child classes */
	virtual void MouseClick(const FInputDeviceRay& ClickPos) {};

	
	/** Protected functions available in base */
	void AssignProperties();
	void SelectPoint(USkeletonPoint* Point);
	void DeselectPoint(USkeletonPoint* Point);
	void DeselectAllPoints();
	void DeleteSelectedPoints();
	void ConnectPoints(USkeletonPoint* Point1, USkeletonPoint* Point2);
	void DisconnectPoints(USkeletonPoint* Point1, USkeletonPoint* Point2);
	//void DisconnectPoints(FSkeletonLine* Line);


private:
	
	FInputRayHit FindRayHit(const FRay& WorldRay, FVector& HitPos);
	//void CreateGizmo(Transform IntialTransform);

protected:

	bool ShiftIsPressed = false;
	bool CtrlIsPressed = false;

	UWorld* TargetWorld = nullptr;
	ASlimeMoldBase* TargetSlimeMoldActor = nullptr;
	std::set<USkeletonPoint*> SelectedPoints;		// set ensures no duplicates
};
