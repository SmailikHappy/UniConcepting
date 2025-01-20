// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlimeMoldEditorToolLineTool.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "BaseBehaviors/ClickDragBehavior.h"

// Custom tool customizations
#include "SlimeMoldEditorToolLineToolCustomization.h"

// for raycast into World
#include "CollisionQueryParams.h"
#include "Engine/World.h"

#include "SceneManagement.h"
#include <Kismet/GameplayStatics.h>

// localization namespace
#define LOCTEXT_NAMESPACE "USlimeMoldEditorToolLineTool"

/*
 * ToolBuilder
 */

UInteractiveTool* USlimeMoldEditorToolLineToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	USlimeMoldEditorToolLineTool* NewTool = NewObject<USlimeMoldEditorToolLineTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

USlimeMoldEditorToolLineToolProperties::USlimeMoldEditorToolLineToolProperties()
{
	
}


void USlimeMoldEditorToolLineTool::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}


void USlimeMoldEditorToolLineTool::Setup()
{
	UInteractiveTool::Setup();

	// Add default mouse input behavior
	UClickDragInputBehavior* MouseBehavior = NewObject<UClickDragInputBehavior>();
	MouseBehavior->Initialize(this);
	AddInputBehavior(MouseBehavior);

	// Create the property set and register it with the Tool
	Properties = NewObject<USlimeMoldEditorToolLineToolProperties>(this);
	AddToolPropertySource(Properties);

	// Register the custom detail customization
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");	
	PropertyModule.RegisterCustomClassLayout(
		USlimeMoldEditorToolLineToolProperties::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FSlimeMoldEditorToolLineToolCustomization::MakeInstance)
	);

	// Get currently selected actor
	OriginActor = Cast<ASlimeMoldActor>(GetSelectedActor());
	if (OriginActor)
	{
		ApplyProperties(OriginActor);
		SplineComponents = OriginActor->GetSplineComponents();
		CurrentSplineState = ESplineCreationState::Extending;
	}

	bHasOriginPoint = true;
}


FInputRayHit USlimeMoldEditorToolLineTool::CanBeginClickDragSequence(const FInputDeviceRay& PressPos)
{
	// we only start drag if press-down is on top of something we can raycast
	FVector Temp;
	FInputRayHit Result = FindRayHit(PressPos.WorldRay, Temp);
	return Result;
}


void USlimeMoldEditorToolLineTool::OnClickPress(const FInputDeviceRay& PressPos)
{
	FVector ClickLocation;
	FInputRayHit HitResult = FindRayHit(PressPos.WorldRay, ClickLocation);

	// Check if the selected actor has changed
	ASlimeMoldActor* NewSelectedActor = Cast<ASlimeMoldActor>(GetSelectedActor());
	if (NewSelectedActor && NewSelectedActor != OriginActor)
	{
		// Update the spline actor and components
		OriginActor = NewSelectedActor;
		SplineComponents = OriginActor->GetSplineComponents();
		CurrentSplineComponent = nullptr;
		CurrentSplineState = ESplineCreationState::Extending;
	}

	if (GEditor)
	{
		GEditor->SelectNone(true, true, false);
		GEditor->SelectActor(OriginActor, true, true);
	}

	if (CurrentSplineState == ESplineCreationState::SettingOrigin)
	{
		// Set the origin point
		OriginPoint = ClickLocation;
		DrawDebugSphereEditor(OriginPoint);
		CurrentSplineState = ESplineCreationState::SettingEndPoint;
	}

	else if (CurrentSplineState == ESplineCreationState::SettingEndPoint)
	{
		// Set the end point and create the initial spline
		int32 NearPointIndex;
		FVector NearPointLocation;

		if (GetNearSplinePoint(ClickLocation, NearPointIndex, NearPointLocation, Properties->SnapThreshold))
		{
			EndPoint = NearPointLocation;
		}
		else
		{
			EndPoint = ClickLocation;
		}

		DrawDebugSphereEditor(EndPoint);

		if (ArePointsOnSameSpline(OriginPoint, EndPoint))
		{
			UE_LOG(LogTemp, Warning, TEXT("Origin and End Points are on the same spline. Spline not created."));
			OriginPoint = EndPoint;
			CurrentSplineState = ESplineCreationState::SettingEndPoint;
			return;
		}

		if (OriginPoint.Equals(EndPoint, 1.0f))
		{
			UE_LOG(LogTemp, Warning, TEXT("Origin and End Point are the same. Spline not created."));
			OriginPoint = EndPoint;
			CurrentSplineState = ESplineCreationState::SettingEndPoint;
			return;
		}

		// Check for intersection
		if (IsLineIntersectingSpline(OriginPoint, EndPoint))
		{
			UE_LOG(LogTemp, Warning, TEXT("Line intersects with an existing spline. Spline not created."));
			OriginPoint = EndPoint;
			CurrentSplineState = ESplineCreationState::Extending;
			return;
		}

		CreateSpline();
		CurrentSplineState = ESplineCreationState::Extending;
	}

	else if (CurrentSplineState == ESplineCreationState::Extending)
	{
		int32 NearPointIndex;
		FVector NearPointLocation;

		if (GetNearSplinePoint(ClickLocation, NearPointIndex, NearPointLocation, Properties->SnapThreshold))
		{
			OriginPoint = NearPointLocation;
			DrawDebugSphereEditor(OriginPoint);
			CurrentSplineState = ESplineCreationState::SettingEndPoint;
		}
	}
}


FInputRayHit USlimeMoldEditorToolLineTool::FindRayHit(const FRay& WorldRay, FVector& HitPos)
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


void USlimeMoldEditorToolLineTool::DrawDebugSphereEditor(const FVector& Location)
{
	if (TargetWorld)
	{
		DrawDebugSphere(TargetWorld, Location, 25.0f, 12, FColor::Red, false, 1.0f);
	}
}


void USlimeMoldEditorToolLineTool::CreateSpline()
{
	if (!TargetWorld) return;

	if (!OriginActor)
	{
		OriginActor = TargetWorld->SpawnActor<ASlimeMoldActor>();

		// Generate a unique name for the actor
		FName NewName = MakeUniqueObjectName(TargetWorld, ASlimeMoldActor::StaticClass(), FName(TEXT("SlimeMoldNetwork")));
		OriginActor->SetActorLabel(NewName.ToString());
	}


	USplineComponent* SplineComponent = NewObject<USplineComponent>(OriginActor);
	SplineComponent->SetupAttachment(OriginActor->GetRootComponent());
	SplineComponent->RegisterComponentWithWorld(TargetWorld);

	SplineComponent->ClearSplinePoints();

	// Add the points to the spline
	SplineComponent->AddSplinePoint(OriginPoint, ESplineCoordinateSpace::World);
	SplineComponent->AddSplinePoint(EndPoint, ESplineCoordinateSpace::World);

	SplineComponent->UpdateSpline();

	if (GEditor)
	{
		GEditor->SelectNone(true, true, false);
		GEditor->SelectActor(OriginActor, true, true);
	}

	OriginPoint = FVector::ZeroVector;
	CurrentSplineComponent = SplineComponent;
	SplineComponents = OriginActor->GetSplineComponents();
	OriginActor->AddSplineComponent(CurrentSplineComponent);
}


bool USlimeMoldEditorToolLineTool::GetNearSplinePoint(const FVector& Location, int32& OutPointIndex, FVector& OutPointLocation, float Threshold)
{
	if (!OriginActor) return false;

	float MinDistance = Threshold;
	bool bFoundNearPoint = false;

	SplineComponents = OriginActor->GetSplineComponents();

	for (USplineComponent* SplineComponent : SplineComponents)
	{
		// iterate through all spline points
		for (int32 i = 0; i < SplineComponent->GetNumberOfSplinePoints(); ++i)
		{
			FVector SplinePoint = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
			float Distance = FVector::Distance(Location, SplinePoint);

			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				OutPointIndex = i;
				OutPointLocation = SplinePoint;
				bFoundNearPoint = true;
			}
		}
	}

	return bFoundNearPoint;
}

bool USlimeMoldEditorToolLineTool::ArePointsOnSameSpline(const FVector& Point1, const FVector& Point2)
{
	if (!OriginActor) return false;

	TArray<USplineComponent*> SplineComponentsArray;
	OriginActor->GetComponents(SplineComponentsArray);

	for (USplineComponent* SplineComponent : SplineComponentsArray)
	{
		int32 Point1Index = INDEX_NONE;
		int32 Point2Index = INDEX_NONE;

		// Iterate through all spline points in the component
		for (int32 i = 0; i < SplineComponent->GetNumberOfSplinePoints(); ++i)
		{
			FVector SplinePoint = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
			if (Point1Index == INDEX_NONE && FVector::Dist(Point1, SplinePoint) < Properties->SnapThreshold)
			{
				Point1Index = i;
			}
			if (Point2Index == INDEX_NONE && FVector::Dist(Point2, SplinePoint) < Properties->SnapThreshold)
			{
				Point2Index = i;
			}

			// If both point are found and belong to this spline return true
			if (Point1Index != INDEX_NONE && Point2Index != INDEX_NONE)
			{
				return true;
			}
		}
	}

	// If no spline component contains both splaines
	return false;
}


AActor* USlimeMoldEditorToolLineTool::GetSelectedActor()
{
	if (GEditor)
	{
		USelection* SelectedActors = GEditor->GetSelectedActors();

		// Check if there are any selected actors
		if (SelectedActors->Num() > 0)
		{
			return Cast<AActor>(SelectedActors->GetSelectedObject(0));
		}
	}
	return nullptr;
}


bool USlimeMoldEditorToolLineTool::IsLineIntersectingSpline(const FVector& LineStart, const FVector& LineEnd)
{
	return false;
}


void USlimeMoldEditorToolLineTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	// if the updated any of the property fields, update the property
	if (OriginActor)
	{
		ApplyProperties(OriginActor);
	}
	else
	{
		ASlimeMoldActor* FoundSplineActor = GetFirstSlimeMoldActor(TargetWorld);
		if (FoundSplineActor)
		{
			ApplyProperties(FoundSplineActor);
		}
	}
}


ASlimeMoldActor* USlimeMoldEditorToolLineTool::GetFirstSlimeMoldActor(UWorld* World)
{
	if (!World) return nullptr;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ASlimeMoldActor::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		return Cast<ASlimeMoldActor>(FoundActors[0]);
	}

	return nullptr;
}

void USlimeMoldEditorToolLineTool::ApplyProperties(ASlimeMoldActor* SlimeMoldActor)
{
	if (SlimeMoldActor)
	{
		SlimeMoldActor->EnableDebugDrawing = Properties->EnableDebugDraw;
		SlimeMoldActor->LineWidth = Properties->Width;
		SlimeMoldActor->LineThickness = Properties->Thickness;
	}
}


#undef LOCTEXT_NAMESPACE
