// Copyright Epic Games, Inc. All Rights Reserved.


#include "SlimeMoldWeakSpotEditingCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "SlimeMoldWeakSpotEditingTool.h"
#include "Engine/Selection.h"

#define LOCTEXT_NAMESPACE "WeakSpotEditingCustomization"

TSharedRef<IDetailCustomization> FSlimeMoldWeakSpotEditingCustomization::MakeInstance()
{
	return MakeShareable(new FSlimeMoldWeakSpotEditingCustomization);
}

void FSlimeMoldWeakSpotEditingCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// Create a new category named options
	IDetailCategoryBuilder& SkeletonButtonsCategory = DetailBuilder.EditCategory("Buttons");

	// Declare properties
	TSharedRef<IPropertyHandle> ResetTransformBoolean = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(USlimeMoldWeakSpotEditingToolProperties, bResetTransformButton));

	// Hide the boolean
	DetailBuilder.HideProperty(ResetTransformBoolean);

	SkeletonButtonsCategory.AddCustomRow(LOCTEXT("ResetTransaformButtonRow", "Reset transform button"))
		.ValueContent()
		[
			SNew(SButton)
				.Text(FText::FromString("Reset"))
				.OnClicked(FOnClicked::CreateLambda([ResetTransformBoolean]()
				{
					// Workaround to update the property value on button click, and trigger button functionality in the tool
					bool bValue = false;
					ResetTransformBoolean->GetValue(bValue);
					ResetTransformBoolean->SetValue(!bValue);

					return FReply::Handled();
				}))
		];
}

#undef LOCTEXT_NAMESPACE