// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlimeMoldMeshEditingTool.h"

#include "SlimeMoldMeshEditingCustomization.h"

// for raycast into World
#include "CollisionQueryParams.h"
#include "Engine/World.h"


// localization namespace
#define LOCTEXT_NAMESPACE "USlimeMoldMeshEditingTool"

/*
 * Tool builder
 */

bool USlimeMoldMeshEditingToolBuilder::CanBuildTool(const FToolBuilderState& SceneState) const
{
	return USlimeMoldEditorFuncLib::SingleActorWithSkeletonComponentIsSelected();
}

UInteractiveTool* USlimeMoldMeshEditingToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	USlimeMoldMeshEditingTool* NewTool = NewObject<USlimeMoldMeshEditingTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

/*
 * Tool logic
 */

void USlimeMoldMeshEditingTool::Setup()
{
	TargetActorComponent = USlimeMoldEditorFuncLib::GetSkeletonComponentFromSelectedActor();
	check(TargetActorComponent);

	UInteractiveTool::Setup();

	// Tool property set
	ToolProperties = NewObject<USlimeMoldMeshEditingToolProperties>(this);
	AddToolPropertySource(ToolProperties);

	ToolProperties->RestoreProperties(this, "MeshEditingToolProperties");

	// Register the custom detail customization
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomClassLayout(
		USlimeMoldMeshEditingToolProperties::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FSlimeMoldMeshEditingCustomization::MakeInstance)
	);

	// If there is a mesh property class assigned, create mesh property set of that class
	if (ToolProperties->MeshPropertyClass)
	{
		// Mesh property set
		MeshProperties = NewObject<USlimeMoldMeshPropertyBase>(this, ToolProperties->MeshPropertyClass);
		check(MeshProperties);

		AddToolPropertySource(MeshProperties);

		MeshProperties->RestoreProperties(this, "MeshEditingMeshProperties");

		MeshProperties->ToolObject = this;

		UE_LOG(LogTemp, Display, TEXT("Mesh properties have been assigned"));
	}

	UE_LOG(LogTemp, Display, TEXT("Mesh editing tool has been initialized"));
}

void USlimeMoldMeshEditingTool::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}

void USlimeMoldMeshEditingTool::OnClickPress(const FInputDeviceRay& PressPos)
{
	// cast ray into world to find hit position
	FVector RayStart = PressPos.WorldRay.Origin;
	FVector RayEnd = PressPos.WorldRay.PointAt(99999999.f);
	FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllObjects);
	FHitResult Result;

	// Trace the ray and select the hit actor
	if (TargetWorld->LineTraceSingleByObjectType(Result, RayStart, RayEnd, QueryParams))
	{
		GEditor->SelectActor(Result.GetActor(), true, true);
	}
}

void USlimeMoldMeshEditingTool::ButtonPressEvent(const FString& ButtonKey)
{
	FEditorScriptExecutionGuard ScriptGuard;
	{
		UE_LOG(LogTemp, Display, TEXT("Custom button event with name: \"%s\" has been called."), *FString(ButtonKey));
		TargetActorComponent->OnCustomButtonPress.Broadcast(MeshProperties, ButtonKey);
	}
}

void USlimeMoldMeshEditingTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	if (!PropertySet) return;

	if (PropertySet == ToolProperties)
	{
		// Mesh property class has been assigned / changed
		if (Property->GetName() == "MeshPropertyClass")
		{
			// Reload the tool, so that MeshProperties with the chosen class would be generated in the panel 
			// For some reason we cannot add new properties to the tool in runtime
			// Huge thanks there is a workaround with a tool reloading
			ToolProperties->SaveProperties(this, "MeshEditingToolProperties");
			GetToolManager()->DeactivateTool(EToolSide::Left, EToolShutdownType::Completed);
			GetToolManager()->ActivateTool(EToolSide::Left);

			return;
		}
	}
}

void USlimeMoldMeshEditingTool::Shutdown(EToolShutdownType ShutdownType)
{
	if (MeshProperties)
	{
		MeshProperties->SaveProperties(this, "MeshEditingMeshProperties");
	}
}

void USlimeMoldMeshEditingTool::OnTick(float DeltaTime)
{
	// Alway check if we are working with the right actor and the actor is valid
	if (USlimeMoldEditorFuncLib::SingleActorWithSkeletonComponentIsSelected())
	{
		if (TargetActorComponent != USlimeMoldEditorFuncLib::GetSkeletonComponentFromSelectedActor())
		{
			// Reload the tool
			GetToolManager()->DeactivateTool(EToolSide::Left, EToolShutdownType::Completed);
			GetToolManager()->ActivateTool(EToolSide::Left);
		}
	}
	else
	{
		GetToolManager()->DeactivateTool(EToolSide::Left, EToolShutdownType::Completed);
	}
}

void USlimeMoldMeshPropertyBase::ButtonPressCall(const FString& Key)
{
	UE_LOG(LogTemp, Display, TEXT("Button %s pressed"), *Key);

	ToolObject->ButtonPressEvent(Key);
}

#undef LOCTEXT_NAMESPACE