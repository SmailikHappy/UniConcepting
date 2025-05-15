// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/ClickDragTool.h"
#include "BaseTools/SingleClickTool.h"

// SlimeMold data structures
#include "SlimeMoldWeakSpotComponent.h"
#include "Structs.h"

// Adding gizmo to the world
#include "BaseGizmos/TransformGizmoUtil.h"
#include "InteractiveToolObjects.h"
#include "InteractiveToolBuilder.h"

// Custom static functions
#include "SlimeMoldEditorToolFunctionLibrary.h"



#include "SlimeMoldWeakSpotEditingTool.generated.h"

/**
 * This is a Tool Builder for our weak spot editor
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API USlimeMoldWeakSpotEditingToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override;
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};



UCLASS(Config = EditorPerProjectUserSettings)
class SLIMEMOLDEDITORTOOL_API USlimeMoldWeakSpotEditingToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	USlimeMoldWeakSpotEditingToolProperties() {}

	UPROPERTY(EditAnywhere, Category = "Buttons")
	bool bResetTransformButton = false;
};



/**
 * Tool logic
 */
UCLASS()
class SLIMEMOLDEDITORTOOL_API USlimeMoldWeakSpotEditingTool : public UInteractiveTool
{
	GENERATED_BODY()

public:
	virtual void SetWorld(UWorld* World);

	/** UInteractiveTool overrides */
	void Setup() override;
	void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;
	void Shutdown(EToolShutdownType ShutdownType) override;
	void Render(IToolsContextRenderAPI* RenderAPI) override;
	void OnTick(float DeltaTime) override;

protected:
	/** Properties of the tool are stored here */
	UPROPERTY()
	TObjectPtr<USlimeMoldWeakSpotEditingToolProperties> Properties;

	/** Debug draw mouse */
	void DrawDebugMouseInfo(const FInputDeviceRay& DevicePos, FColor Color);

	bool bDrawDebugMouseInfo = false;

	/** Helper functions */
	void ToolPseudoReload();
	void GenerateSphereVertecis();
	void DrawSphere(FPrimitiveDrawInterface* PDI, const FTransform& Transform, const float Radius, const FColor& Color);

private:
	
	/** Gizmo functionality */
	void CreateGizmo();
	void DestroyGizmo();

	UPROPERTY()
	UCombinedTransformGizmo* TransformGizmo = nullptr;

	FVector PreviousGizmoWorldLocation = FVector::ZeroVector;
	FVector GizmoWorldPositionDelta = FVector::ZeroVector;

protected:	

	FInputDeviceRay MouseRayWhenPressed;
	FInputDeviceRay MouseRayWhenReleased;

	TArray<FVector> SphereVertices;

	UPROPERTY()
	UWorld* TargetWorld = nullptr;

	UPROPERTY()
	AActor* TargetActor = nullptr;
	
	UPROPERTY()
	USlimeMoldWeakSpotComponent* TargetActorComponent = nullptr;
};
