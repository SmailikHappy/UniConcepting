// Copyright Epic Games, Inc. All Rights Reserved.


#include "SlimeMoldEditorToolWorldManagingCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "SlimeMoldEditorToolWorldManaging.h"
#include "Engine/Selection.h"
#include "LevelEditorViewport.h"


TSharedPtr<USlimeMoldEditorToolWorldManaging> FSlimeMoldEditorToolWorldManagingCustomization::SlimeMoldEditorToolWorldManaging = nullptr;

TSharedRef<IDetailCustomization> FSlimeMoldEditorToolWorldManagingCustomization::MakeInstance()
{
	return MakeShareable(new FSlimeMoldEditorToolWorldManagingCustomization);
}

void FSlimeMoldEditorToolWorldManagingCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// Create a new category named options
	IDetailCategoryBuilder& OptionsCategory = DetailBuilder.EditCategory("boooooom category");

	// Add properties
	TSharedRef<IPropertyHandle> GeometryToAffectProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(USlimeMoldEditorToolWorldManagingProperties, GeometryToAffect));
	OptionsCategory.AddProperty(GeometryToAffectProperty);

	USlimeMoldEditorToolWorldManagingProperties* WorldManaginToolsProperties = nullptr;

	/** Get Properties */
	/*TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);
	for (TWeakObjectPtr<UObject> Object : CustomizedObjects)
	{
		if (Object.IsValid())
		{
			WorldManaginToolsProperties = Cast<USlimeMoldEditorToolWorldManagingProperties>(Object);
			if (WorldManaginToolsProperties)
				break;
		}
	}

	check(WorldManaginToolsProperties);*/

	OptionsCategory.AddCustomRow(FText::FromString("usuless text"))
		.ValueContent()
		[
			SNew(SButton)
				.Text(FText::FromString("Add"))
				.OnClicked_UObject(SlimeMoldEditorToolWorldManaging.Get(), &USlimeMoldEditorToolWorldManaging::AddActorToGeometryCache)
		];

	OptionsCategory.AddCustomRow(FText::FromString("usuless text"))
		.ValueContent()
		[
			SNew(SButton)
				.Text(FText::FromString("Remove"))
				.OnClicked_UObject(SlimeMoldEditorToolWorldManaging.Get(), &USlimeMoldEditorToolWorldManaging::RemoveActorFromGeometryCache)
		];
}
