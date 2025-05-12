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
	/*FName ButtonVariablesCategoryName = FName("Button");
	IDetailCategoryBuilder& ButtonCategory = DetailBuilder.EditCategory(ButtonVariablesCategoryName);
	DetailBuilder.HideCategory(ButtonVariablesCategoryName);*/

	// Create a new category named options
	//IDetailCategoryBuilder& MeshButtonsCategory = DetailBuilder.EditCategory("Buttons");

	//TSharedRef<IPropertyHandle> PressButtonProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(USlimeMoldMeshEditingToolProperties, bButtonPressed));
	//TSharedRef<IPropertyHandle> ButtonKeyProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(USlimeMoldMeshEditingToolProperties, ButtonKey));

	// Hide the properties themselves and use them for the workaround
	//DetailBuilder.HideProperty(PressButtonProperty);
	//DetailBuilder.HideProperty(ButtonKeyProperty);
}

#undef LOCTEXT_NAMESPACE