// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/ClickDragTool.h"
#include "Components/SplineComponent.h"
#include "SlimeMoldActor.h"
#include "SlimeMoldEditorToolLineTool.generated.h"


/**
 * Builder for USlimeMoldEditorToolLineTool
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API USlimeMoldEditorToolLineToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};


/**
 * Property set for the USlimeMoldEditorToolLineTool
 */
UCLASS(Config = EditorPerProjectUserSettings)
class SLIMEMOLDEDITORTOOL_API USlimeMoldEditorToolLineToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	USlimeMoldEditorToolLineToolProperties();

	UPROPERTY(EditAnywhere, Category = "Spline network")
	bool EnableDebugDraw = true;

	UPROPERTY(EditAnywhere, Category = "Spline network", meta = (ClampMin = "0.0"))
	float Width = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Spline network", meta = (ClampMin = "0.0"))
	float Thickness = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Spline network", meta = (ClampMin = "0.0"))
	float SnapThreshold = 150.0f;
};



/**
 * USlimeMoldEditorToolLineTool is an example Tool that allows the user to measure the
 * distance between two points. The first point is set by click-dragging the mouse, and
 * the second point is set by shift-click-dragging the mouse.
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API USlimeMoldEditorToolLineTool : public UInteractiveTool, public IClickDragBehaviorTarget
{
	GENERATED_BODY()

public:
	virtual void SetWorld(UWorld* World);

	/** ULineTool overrides */
	virtual void Setup() override;
	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;


	/** IClickDragBehaviorTarget implementation */
	virtual FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override;
	virtual void OnClickPress(const FInputDeviceRay& PressPos) override;
	// these are not used in this Tool
	virtual void OnClickRelease(const FInputDeviceRay& ReleasePos) override {}
	virtual void OnTerminateDragSequence() override {}
	virtual void OnClickDrag(const FInputDeviceRay& DragPos) override {}


protected:
	/** Properties of the tool are stored here */
	UPROPERTY()
	TObjectPtr<USlimeMoldEditorToolLineToolProperties> Properties;


protected:
	enum class ESplineCreationState
	{
		SettingOrigin,
		SettingEndPoint,
		Extending
	};

	UWorld* TargetWorld = nullptr;		// target World we will raycast into

	bool bHasOriginPoint;
	ASlimeMoldActor* OriginActor;
	TArray<USplineComponent*> SplineComponents;
	USplineComponent* CurrentSplineComponent = nullptr;

	/* Start and end points of the spline */
	FVector OriginPoint;
	FVector EndPoint;

	ESplineCreationState CurrentSplineState = ESplineCreationState::SettingOrigin;

	FInputRayHit FindRayHit(const FRay& WorldRay, FVector& HitPos);		// raycasts into World
	void DrawDebugSphereEditor(const FVector& Location);
	void CreateSpline();
	bool GetNearSplinePoint(const FVector& Location, int32& OutPointIndex, FVector& OutPointLocation, float Threshold);
	bool ArePointsOnSameSpline(const FVector& Point1, const FVector& Point2);
	AActor* GetSelectedActor();
	bool IsLineIntersectingSpline(const FVector& LineStart, const FVector& LineEnd);
	//bool DoLinesIntersect(const FVector& A1, const FVector& A2, const FVector& B1, const FVector& B2);
	ASlimeMoldActor* GetFirstSlimeMoldActor(UWorld* World);
	void ApplyProperties(ASlimeMoldActor* SlimeMoldActor);

	static const int MoveSecondPointModifierID = 1;		// identifier we associate with the shift key
	bool bSecondPointModifierDown = false;				// flag we use to keep track of modifier state
	bool bMoveSecondPoint = false;						// flag we use to keep track of which point we are moving during a press-drag
};
