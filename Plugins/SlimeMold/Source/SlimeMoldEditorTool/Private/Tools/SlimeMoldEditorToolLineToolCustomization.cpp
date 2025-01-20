// Copyright Epic Games, Inc. All Rights Reserved.


#include "SlimeMoldEditorToolLineToolCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "SlimeMoldEditorToolLineTool.h"
#include "Engine/Selection.h"

TSharedRef<IDetailCustomization> FSlimeMoldEditorToolLineToolCustomization::MakeInstance()
{
	return MakeShareable(new FSlimeMoldEditorToolLineToolCustomization);
}

void FSlimeMoldEditorToolLineToolCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// Create a new category named options
	IDetailCategoryBuilder& OptionsCategory = DetailBuilder.EditCategory("Spline network");

	// Add properties

	TSharedRef<IPropertyHandle> EnableDebugDrawProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(USlimeMoldEditorToolLineToolProperties, EnableDebugDraw));
	OptionsCategory.AddProperty(EnableDebugDrawProperty);

	TSharedRef<IPropertyHandle> WidthProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(USlimeMoldEditorToolLineToolProperties, Width));
	OptionsCategory.AddProperty(WidthProperty);

	TSharedRef<IPropertyHandle> ThicknessProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(USlimeMoldEditorToolLineToolProperties, Thickness));
	OptionsCategory.AddProperty(ThicknessProperty);

	TSharedRef<IPropertyHandle> SnapThresholdProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(USlimeMoldEditorToolLineToolProperties, SnapThreshold));
	OptionsCategory.AddProperty(SnapThresholdProperty);

	// Add the "create" button
	OptionsCategory.AddCustomRow(FText::FromString("Create button"))
		.ValueContent()
		[
			SNew(SButton)
				.Text(FText::FromString("Create"))
				.OnClicked(FOnClicked::CreateSP(this, &FSlimeMoldEditorToolLineToolCustomization::OnCreateButtonClicked))
		];
}

FReply FSlimeMoldEditorToolLineToolCustomization::OnCreateButtonClicked()
{
	// Handle the button click here
	UE_LOG(LogTemp, Warning, TEXT("Create button clicked"));
	return FReply::Handled();
}