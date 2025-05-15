// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlimeMoldEditorToolFunctionLibrary.h"
#include "Selection.h"


bool USlimeMoldEditorFuncLib::SingleActorWithSkeletonComponentIsSelected()
{
	TArray<UObject*> SelectedObjects;
	GEditor->GetSelectedActors()->GetSelectedObjects(SelectedObjects);

	if (SelectedObjects.Num() != 1)
	{
		return false;
	}

	AActor* SelectedActor = Cast<AActor>(SelectedObjects[0]);

	if (!SelectedActor)
	{
		return false;
	}

	if (SelectedActor->GetComponentByClass<USlimeMoldSkeletonComponent>() == nullptr)
	{
		return false;
	}

	return true;
}

USlimeMoldSkeletonComponent* USlimeMoldEditorFuncLib::GetSkeletonComponentFromSelectedActor()
{
	if (SingleActorWithSkeletonComponentIsSelected())
	{
		TArray<UObject*> SelectedObjects;
		GEditor->GetSelectedActors()->GetSelectedObjects(SelectedObjects);
		return Cast<AActor>(SelectedObjects[0])->GetComponentByClass<USlimeMoldSkeletonComponent>();
	}

	return nullptr;
}

bool USlimeMoldEditorFuncLib::SingleActorWithWeakSpotComponentIsSelected()
{
	TArray<UObject*> SelectedObjects;
	GEditor->GetSelectedActors()->GetSelectedObjects(SelectedObjects);
	if (SelectedObjects.Num() != 1)
	{
		return false;
	}
	AActor* SelectedActor = Cast<AActor>(SelectedObjects[0]);
	if (!SelectedActor)
	{
		return false;
	}
	if (SelectedActor->GetComponentByClass<USlimeMoldWeakSpotComponent>() == nullptr)
	{
		return false;
	}
	return true;
}

USlimeMoldWeakSpotComponent* USlimeMoldEditorFuncLib::GetWeakSpotComponentFromSelectedActor()
{
	if (SingleActorWithWeakSpotComponentIsSelected())
	{
		TArray<UObject*> SelectedObjects;
		GEditor->GetSelectedActors()->GetSelectedObjects(SelectedObjects);
		return Cast<AActor>(SelectedObjects[0])->GetComponentByClass<USlimeMoldWeakSpotComponent>();
	}
	return nullptr;
}

AActor* USlimeMoldEditorFuncLib::GetSingleSelectedActor()
{
	if (SingleActorWithSkeletonComponentIsSelected())
	{
		TArray<UObject*> SelectedObjects;
		GEditor->GetSelectedActors()->GetSelectedObjects(SelectedObjects);
		return Cast<AActor>(SelectedObjects[0]);
	}
	return nullptr;
}

void USlimeMoldEditorFuncLib::WarnUserDialog(const FString& Title, const FString& Message)
{
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Message), FText::FromString(Title));
}

FInputRayHit USlimeMoldEditorFuncLib::FindRayHit(const UWorld* TargetWorld, const FRay& WorldRay)
{
	// Trace a ray into the World
	FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllObjects);
	FHitResult Result;

	bool bRayWasHit = TargetWorld->LineTraceSingleByObjectType(Result, WorldRay.Origin, WorldRay.PointAt(999999), QueryParams);

	if (bRayWasHit)
	{
		return FInputRayHit(Result.Distance);
	}

	return FInputRayHit();
}

bool USlimeMoldEditorFuncLib::FindRayHitPos(const UWorld* TargetWorld, const FRay& WorldRay, FVector& HitPos)
{
	// Trace a ray into the World
	FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllObjects);
	FHitResult Result;

	bool bHitDetected = TargetWorld->LineTraceSingleByObjectType(Result, WorldRay.Origin, WorldRay.PointAt(999999), QueryParams);

	if (bHitDetected)
	{
		HitPos = Result.ImpactPoint;
	}

	return bHitDetected;
}