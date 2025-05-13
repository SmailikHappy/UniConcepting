// Copyright Epic Games, Inc. All Rights Reserved.


#include "SlimeMoldMeshEditingCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "SlimeMoldMeshEditingTool.h"
#include "Engine/Selection.h"

#define LOCTEXT_NAMESPACE "MeshEditingCustomization"

TSharedRef<IDetailCustomization> FSlimeMoldMeshEditingCustomization::MakeInstance()
{
	return MakeShareable(new FSlimeMoldMeshEditingCustomization);
}

void FSlimeMoldMeshEditingCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{

}

#undef LOCTEXT_NAMESPACE