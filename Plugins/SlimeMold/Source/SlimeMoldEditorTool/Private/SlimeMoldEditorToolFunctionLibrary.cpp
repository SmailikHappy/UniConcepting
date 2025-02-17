// Copyright Epic Games, Inc. All Rights Reserved.
#include "SlimeMoldEditorToolFunctionLibrary.h"
#include "Selection.h"
#include "SlimeMoldBase.h"

bool USlimeMoldEditorFuncLib::SingleSlimeMoldObjectIsSelected()
{
	TArray<UObject*> SelectedObjects;
	GEditor->GetSelectedActors()->GetSelectedObjects(SelectedObjects);

	if (SelectedObjects.Num() != 1)
	{
		return false;
	}

	if (Cast<ASlimeMoldBase>(SelectedObjects[0]) == nullptr)
	{
		return false;
	}

	return true;
}

ASlimeMoldBase* USlimeMoldEditorFuncLib::GetSingleSelectedSlimeMoldObject()
{
	if (SingleSlimeMoldObjectIsSelected())
	{
		TArray<UObject*> SelectedObjects;
		GEditor->GetSelectedActors()->GetSelectedObjects(SelectedObjects);
		return Cast<ASlimeMoldBase>(SelectedObjects[0]);
	}

	return nullptr;
}
