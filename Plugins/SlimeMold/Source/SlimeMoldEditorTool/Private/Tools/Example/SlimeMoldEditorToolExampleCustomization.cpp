// Copyright Epic Games, Inc. All Rights Reserved.


#include "SlimeMoldEditorToolExampleCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "SlimeMoldEditorToolExample.h"
#include "Engine/Selection.h"

TSharedRef<IDetailCustomization> FSlimeMoldEditorToolExampleCustomization::MakeInstance()
{
	return MakeShareable(new FSlimeMoldEditorToolExampleCustomization);
}

void FSlimeMoldEditorToolExampleCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// Create a new category named options
	IDetailCategoryBuilder& OptionsCategory = DetailBuilder.EditCategory("boooooom category");

	// Add properties
	TSharedRef<IPropertyHandle> EnableDebugDrawProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(USlimeMoldEditorToolExampleProperties, SomeProperty));
	OptionsCategory.AddProperty(EnableDebugDrawProperty);


	// Add the funny button
	OptionsCategory.AddCustomRow(FText::FromString("A funny button"))
		.ValueContent()
		[
			SNew(SButton)
				.Text(FText::FromString("Funny button"))
				.OnClicked(FOnClicked::CreateSP(this, &FSlimeMoldEditorToolExampleCustomization::OnFunnyButtonClicked))
		];
}

FReply FSlimeMoldEditorToolExampleCustomization::OnFunnyButtonClicked()
{
	// Handle the button click here
	UE_LOG(LogTemp, Warning, TEXT("Funny button has been clicked"));
	return FReply::Handled();
}