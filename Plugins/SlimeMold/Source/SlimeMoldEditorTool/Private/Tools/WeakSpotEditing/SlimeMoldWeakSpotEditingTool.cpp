// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlimeMoldWeakSpotEditingTool.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "BaseBehaviors/SingleClickBehavior.h"
#include "BaseBehaviors/ClickDragBehavior.h"
#include "BaseBehaviors/MouseHoverBehavior.h"

#include "SlimeMoldWeakSpotEditingCustomization.h"

// for raycast into World
#include "CollisionQueryParams.h"
#include "Engine/World.h"

#include "SceneManagement.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>

#include "CustomMacros.h"





// localization namespace
#define LOCTEXT_NAMESPACE "USlimeMoldWeakSpotEditingTool"


/*
 * Tool builder
 */

bool USlimeMoldWeakSpotEditingToolBuilder::CanBuildTool(const FToolBuilderState& SceneState) const
{
	return USlimeMoldEditorFuncLib::SingleActorWithWeakSpotComponentIsSelected();
}

UInteractiveTool* USlimeMoldWeakSpotEditingToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	USlimeMoldWeakSpotEditingTool* NewTool = NewObject<USlimeMoldWeakSpotEditingTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}



/*
 * Tool logic
 */

void USlimeMoldWeakSpotEditingTool::Setup()
{
	TargetActor = USlimeMoldEditorFuncLib::GetSingleSelectedActor();
	check(TargetActor); 

	TargetActorComponent = USlimeMoldEditorFuncLib::GetWeakSpotComponentFromSelectedActor();
	check(TargetActorComponent);

	UInteractiveTool::Setup();



	Properties = NewObject<USlimeMoldWeakSpotEditingToolProperties>(this);
	AddToolPropertySource(Properties);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomClassLayout(
		USlimeMoldWeakSpotEditingToolProperties::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FSlimeMoldWeakSpotEditingCustomization::MakeInstance)
	);

	GenerateSphereVertecis();

	CreateGizmo();

	UE_LOG(LogTemp, Display, TEXT("WeakSpot editing tool has been initialized"));
}

void USlimeMoldWeakSpotEditingTool::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}

void USlimeMoldWeakSpotEditingTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	if (!PropertySet) return;

	if (PropertySet != Properties) return;

	if (Property->GetName() == "bResetTransformButton") {
		TransformGizmo->SetNewGizmoTransform(TargetActor->GetActorTransform());
	}
}

void USlimeMoldWeakSpotEditingTool::Shutdown(EToolShutdownType ShutdownType)
{
	Properties->SaveProperties(this, "WeakSpot properties");
	DestroyGizmo();
}


/*
 * Helper functions
 */

void USlimeMoldWeakSpotEditingTool::ToolPseudoReload()
{
	if (USlimeMoldEditorFuncLib::SingleActorWithWeakSpotComponentIsSelected())
	{
		TargetActorComponent = USlimeMoldEditorFuncLib::GetWeakSpotComponentFromSelectedActor();
		TargetActor = USlimeMoldEditorFuncLib::GetSingleSelectedActor();
	}

	// For safety checks
	if (!TargetActor || !TargetActorComponent)
	{
		GetToolManager()->DeactivateTool(EToolSide::Left, EToolShutdownType::Completed);
		UE_LOG(LogTemp, Warning, TEXT("Tool pseudo reload failed!"));
	}
}

void USlimeMoldWeakSpotEditingTool::GenerateSphereVertecis()
{
	int iterations = 16;
	float k = 2.0f * PI / (float)iterations;

	SphereVertices.Empty();
	SphereVertices.Reserve(iterations + 1);

	for (int i = 0; i < iterations; ++i) {
		const float phi = i * k;
		float positionX = cos(phi);
		float positionY = sin(phi);
		SphereVertices.Add(FVector(positionX, positionY, 0.0f));
	}

	SphereVertices.Add(FVector(1.0f, 0.0f, 0.0f)); // Add the first vertex to close the loop
}

void USlimeMoldWeakSpotEditingTool::DrawSphere(FPrimitiveDrawInterface* PDI, const FTransform& Transform, const float Radius, const FColor& Color)
{
	FTransform trans1 = FTransform(FRotator(0.0f, 0.0f, 90.0f));
	FTransform trans2 = FTransform(FRotator(0.0f, 90.0f, 0.0f));
	FTransform trans3 = FTransform(FRotator(90.0f, 0.0f, 0.0f));


	for (int i = 1; i < SphereVertices.Num(); i++)
	{
		FVector Vertex1Pos = Radius * SphereVertices[i];
		FVector Vertex2Pos = Radius * SphereVertices[i-1];

		FVector Vertex1PosTransformed1 = UKismetMathLibrary::TransformLocation(Transform, UKismetMathLibrary::TransformLocation(trans1, Vertex1Pos));
		FVector Vertex2PosTransformed1 = UKismetMathLibrary::TransformLocation(Transform, UKismetMathLibrary::TransformLocation(trans1, Vertex2Pos));

		FVector Vertex1PosTransformed2 = UKismetMathLibrary::TransformLocation(Transform, UKismetMathLibrary::TransformLocation(trans2, Vertex1Pos));
		FVector Vertex2PosTransformed2 = UKismetMathLibrary::TransformLocation(Transform, UKismetMathLibrary::TransformLocation(trans2, Vertex2Pos));

		FVector Vertex1PosTransformed3 = UKismetMathLibrary::TransformLocation(Transform, UKismetMathLibrary::TransformLocation(trans3, Vertex1Pos));
		FVector Vertex2PosTransformed3 = UKismetMathLibrary::TransformLocation(Transform, UKismetMathLibrary::TransformLocation(trans3, Vertex2Pos));

		PDI->DrawLine(Vertex1PosTransformed1, Vertex2PosTransformed1, Color, SDPG_Foreground);
		PDI->DrawLine(Vertex1PosTransformed2, Vertex2PosTransformed2, Color, SDPG_Foreground);
		PDI->DrawLine(Vertex1PosTransformed3, Vertex2PosTransformed3, Color, SDPG_Foreground);
	}
}

/*
 * Debug drawing
 */
void USlimeMoldWeakSpotEditingTool::DrawDebugMouseInfo(const FInputDeviceRay& DevicePos, FColor Color)
{
	if (!bDrawDebugMouseInfo) return;

	FVector WorldPosToDraw;
	USlimeMoldEditorFuncLib::FindRayHitPos(TargetWorld, DevicePos.WorldRay, WorldPosToDraw);
	DrawDebugPoint(TargetWorld, WorldPosToDraw, 10.0f, Color, false, 0.1f);
}

/*
 * Gizmo functionality
 */
void USlimeMoldWeakSpotEditingTool::DestroyGizmo()
{
	UInteractiveGizmoManager* const GizmoManager = GetToolManager()->GetPairedGizmoManager();
	ensure(GizmoManager);

	if (!TransformGizmo) return;

	GizmoManager->DestroyGizmo(TransformGizmo);
	TransformGizmo = nullptr;
}


void USlimeMoldWeakSpotEditingTool::CreateGizmo()
{
	UInteractiveGizmoManager* const GizmoManager = GetToolManager()->GetPairedGizmoManager();
	ensure(GizmoManager);

	UTransformProxy* GizmoProxy = NewObject<UTransformProxy>(this);
	ensure(GizmoProxy);
	GizmoProxy->SetTransform(TargetActorComponent->WeakSpotSphereTransform * TargetActor->GetActorTransform());

	TransformGizmo = GizmoManager->CreateCustomTransformGizmo(
		ETransformGizmoSubElements::ScaleAllAxes | ETransformGizmoSubElements::TranslateAllAxes | ETransformGizmoSubElements::TranslateAllPlanes | ETransformGizmoSubElements::RotateAllAxes,
		this);
	ensure(TransformGizmo);

	//TransformGizmo->ActiveGizmoMode = EToolContextTransformGizmoMode::Combined;
	TransformGizmo->CurrentCoordinateSystem = EToolContextCoordinateSystem::Local;
	TransformGizmo->bUseContextGizmoMode = false;
	TransformGizmo->bUseContextCoordinateSystem = false;
	TransformGizmo->SetActiveTarget(GizmoProxy, GetToolManager());


	GizmoProxy->OnTransformChanged.AddWeakLambda(this, [this](UTransformProxy*, FTransform NewTransform)
		{
			TargetActorComponent->WeakSpotSphereTransform = NewTransform * TargetActor->GetActorTransform().Inverse();
		}
	);
}

/*
 * Rendering
 */
void USlimeMoldWeakSpotEditingTool::Render(IToolsContextRenderAPI* RenderAPI)
{
	FPrimitiveDrawInterface* PDI = RenderAPI->GetPrimitiveDrawInterface();
	
	DrawSphere(PDI, TargetActorComponent->WeakSpotSphereTransform * TargetActor->GetActorTransform(), 100.0f, FColor::Red);
}

void USlimeMoldWeakSpotEditingTool::OnTick(float DeltaTime)
{
	// Alway check if we are working with the right actor and the actor is valid
	if (USlimeMoldEditorFuncLib::SingleActorWithSkeletonComponentIsSelected())
	{
		if (TargetActorComponent != USlimeMoldEditorFuncLib::GetWeakSpotComponentFromSelectedActor())
		{
			ToolPseudoReload();
		}
	}
	else
	{
		GetToolManager()->DeactivateTool(EToolSide::Left, EToolShutdownType::Completed);
	}
}


#undef LOCTEXT_NAMESPACE
