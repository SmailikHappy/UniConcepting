// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlimeMoldEditorToolSourceManagingCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"

// Widgets
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

#include "SlimeMoldEditorToolSourceManaging.h"

// localization namespace
#define LOCTEXT_NAMESPACE "USlimeMoldEditorToolSourceManaging"

/*
 * ToolBuilder
 */

TSharedPtr<USlimeMoldEditorToolSourceManaging> FSlimeMoldEditorToolSourceManagingCustomization::SlimeMoldEditorToolSourceManaging = nullptr;

TSharedRef<IDetailCustomization> FSlimeMoldEditorToolSourceManagingCustomization::MakeInstance()
{
	return MakeShareable(new FSlimeMoldEditorToolSourceManagingCustomization);
}

void FSlimeMoldEditorToolSourceManagingCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// Create a new category named options
	IDetailCategoryBuilder& OptionsCategory = DetailBuilder.EditCategory("boooooom category");

	// Add properties
	TSharedRef<IPropertyHandle> SomeProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(USlimeMoldEditorToolSourceManagingProperties, SomeProperty));
	OptionsCategory.AddProperty(SomeProperty);

	USlimeMoldEditorToolSourceManagingProperties* SourceManaginToolsProperties = nullptr;

	/** Get Properties */
	/*TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);
	for (TWeakObjectPtr<UObject> Object : CustomizedObjects)
	{
		if (Object.IsValid())
		{
			SourceManaginToolsProperties = Cast<USlimeMoldEditorToolSourceManagingProperties>(Object);
			if (SourceManaginToolsProperties)
				break;
		}
	}

	check(SourceManaginToolsProperties);*/

	// Add properties
	/*TSharedRef<IPropertyHandle> SomeProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(USlimeMoldEditorToolSourceManagingProperties, SomeProperty));
	OptionsCategory.AddProperty(SomeProperty);*/

	TSharedRef<IPropertyHandle> RadiusProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(USlimeMoldEditorToolSourceManagingProperties, Radius));
	OptionsCategory.AddProperty(RadiusProperty);

	TSharedRef<IPropertyHandle> BranchCountProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(USlimeMoldEditorToolSourceManagingProperties, BranchCount));
	OptionsCategory.AddProperty(BranchCountProperty);

	OptionsCategory.AddCustomRow(FText::FromString("Shutdown button"))
		.ValueContent()
		[
			SNew(SButton)
				.Text(FText::FromString("Place the Slime Mold Source"))
				.OnClicked_UObject(SlimeMoldEditorToolSourceManaging.Get(), &USlimeMoldEditorToolSourceManaging::AcceptSlimeMoldSource)
		];
}


#undef LOCTEXT_NAMESPACE
