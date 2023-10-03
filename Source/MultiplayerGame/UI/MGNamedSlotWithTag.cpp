// Copyright Artem Volovyi. All Rights Reserved.


#include "MGNamedSlotWithTag.h"

#include "GameplayTagContainer.h"

TSharedRef<SWidget> UMGNamedSlotWithTag::RebuildWidget()
{
	if (IsDesignTime())
	{
		auto GetExtensionPointText = [this]()
		{
			return FText::FromName(SlotTag.GetTagName());
		};

		TSharedRef<SOverlay> MessageBox = SNew(SOverlay);

		MessageBox->AddSlot()
			.Padding(5.0f)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Justification(ETextJustify::Center)
				.Text_Lambda(GetExtensionPointText)
			];

		return MessageBox;
	}

	return Super::RebuildWidget();
}
