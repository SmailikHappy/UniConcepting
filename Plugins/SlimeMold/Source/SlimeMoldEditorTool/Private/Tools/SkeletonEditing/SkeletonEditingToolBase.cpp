// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkeletonEditingToolBase.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "BaseBehaviors/SingleClickBehavior.h"

// for raycast into World
#include "CollisionQueryParams.h"
#include "Engine/World.h"

#include "SceneManagement.h"
#include <Kismet/GameplayStatics.h>



// localization namespace
#define LOCTEXT_NAMESPACE "USkeletonEditingToolBase"

/*
 * Tool logic
 */

void USkeletonEditingToolBase::Setup()
{
	UInteractiveTool::Setup();

	// Add default mouse input behavior
	USingleClickInputBehavior* SingleClickBehavior = NewObject<USingleClickInputBehavior>();
	SingleClickBehavior->Initialize(this);
	AddInputBehavior(SingleClickBehavior);

	SingleClickBehavior->Modifiers.RegisterModifier(1, FInputDeviceState::IsShiftKeyDown);
	SingleClickBehavior->Modifiers.RegisterModifier(2, FInputDeviceState::IsCtrlKeyDown);
}

void USkeletonEditingToolBase::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}

void USkeletonEditingToolBase::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	if (!PropertySet) return;

	if (PropertySet != Properties) return;

	UE_LOG(LogTemp, Warning, TEXT("Property modified"));
}

void USkeletonEditingToolBase::AssignProperties()
{
	// Create the property set and register it with the Tool
	Properties = NewObject<USkeletonEditingToolBaseProperties>(this);
	AddToolPropertySource(Properties);

	UE_LOG(LogTemp, Warning, TEXT("Properties assigned"));

	// Register the custom detail customization
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
}

void USkeletonEditingToolBase::SelectPoint(USkeletonPoint* Point)
{
	if (SelectedPoints.empty())
	{
		// Create a gizmo
	}

	SelectedPoints.insert(Point);
}

void USkeletonEditingToolBase::DeselectPoint(USkeletonPoint* Point)
{
	if (SelectedPoints.find(Point) != SelectedPoints.end())
	{
		SelectedPoints.erase(Point);
	}

	if (SelectedPoints.empty())
	{
		// Destroy the gizmo
	}
}

void USkeletonEditingToolBase::DeselectAllPoints()
{
	SelectedPoints.clear();
	// Destroy the gizmo
}

void USkeletonEditingToolBase::DeleteSelectedPoints()
{
	std::set<int32> LinesIndicesToRemove;
	for (USkeletonPoint* Point : SelectedPoints)
	{
		// Remove all lines connected to this point
		/*for (int i = 0; i < TargetSlimeMoldActor->SkeletonLines.Num(); i++)
		{
			FSkeletonLine& Line = TargetSlimeMoldActor->SkeletonLines[i];

			if (&TargetSlimeMoldActor->SkeletonPoints[Line.PointIndex1] == Point ||
				&TargetSlimeMoldActor->SkeletonPoints[Line.PointIndex2] == Point)
			{
				LinesIndicesToRemove.insert(i);
			}
		}*/

		//TargetSlimeMoldActor->SkeletonLines.RemoveAll([Point](FSkeletonLine Line) {
		//	return Line.Point1 == Point || Line.Point2 == Point;
		//	});

		for (int i = 0; i < TargetSlimeMoldActor->SkeletonLines.Num(); i++)
		{
			FSkeletonLine& Line = TargetSlimeMoldActor->SkeletonLines[i];
			if (Line.Point1 == Point || Line.Point2 == Point)
			{
				TargetSlimeMoldActor->SkeletonLines.RemoveAt(i--);
			}
		}
	}
	
	DeselectAllPoints();
}

void USkeletonEditingToolBase::ConnectPoints(USkeletonPoint* Point1, USkeletonPoint* Point2)
{
}

void USkeletonEditingToolBase::DisconnectPoints(USkeletonPoint* Point1, USkeletonPoint* Point2)
{
}

FInputRayHit USkeletonEditingToolBase::FindRayHit(const FRay& WorldRay, FVector& HitPos)
{
	// trace a ray into the World
	FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllObjects);
	FHitResult Result;
	bool bHitWorld = TargetWorld->LineTraceSingleByObjectType(Result, WorldRay.Origin, WorldRay.PointAt(999999), QueryParams);
	if (bHitWorld)
	{
		HitPos = Result.ImpactPoint;
		return FInputRayHit(Result.Distance);
	}
	return FInputRayHit();
}

void USkeletonEditingToolBase::Render(IToolsContextRenderAPI* RenderAPI)
{
	if (TargetSlimeMoldActor)
	{
		// Render the skeleton with debug view
		FPrimitiveDrawInterface* PDI = RenderAPI->GetPrimitiveDrawInterface();

		for (const FSkeletonLine& Line : TargetSlimeMoldActor->SkeletonLines)
		{
			PDI->DrawLine(Line.Point1->WorldPos, Line.Point2->WorldPos,
						  FLinearColor(0.0f, 1.0f, 1.0f, 1.0f), SDPG_Foreground, Properties->DebugLineThickness);
		}

		for (USkeletonPoint* Point : TargetSlimeMoldActor->SkeletonPoints)
		{
			FLinearColor PointColor = SelectedPoints.find(Point) != SelectedPoints.end() ? FLinearColor::Red : FLinearColor::White;
			
			PDI->DrawPoint(Point->WorldPos, FLinearColor::White, Properties->DebugPointSize, SDPG_Foreground);
		}
	}
}

FInputRayHit USkeletonEditingToolBase::IsHitByClick(const FInputDeviceRay& ClickPos)
{
	FVector RayStart = ClickPos.WorldRay.Origin;
	FVector RayEnd = ClickPos.WorldRay.PointAt(99999999.f);
	FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllObjects);
	FHitResult Result;
	FInputRayHit Hit;


	if (TargetWorld->LineTraceSingleByObjectType(Result, RayStart, RayEnd, QueryParams))
	{

		Hit.bHit = true;
		Hit.HitDepth = Result.Distance;
	}
	else
	{
		Hit.bHit = false;
	}

	return Hit;
}

void USkeletonEditingToolBase::OnClicked(const FInputDeviceRay& ClickPos)
{
	MouseClick(ClickPos);
}

void USkeletonEditingToolBase::OnUpdateModifierState(int ModifierID, bool bIsOn)
{
	if (ModifierID == 1)
	{
		ShiftIsPressed = bIsOn;
	}
	if (ModifierID == 2)
	{
		CtrlIsPressed = bIsOn;
	}
}

#undef LOCTEXT_NAMESPACE
