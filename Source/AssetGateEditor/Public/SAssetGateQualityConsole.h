// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Slate widget that displays the latest validation diagnostics for the active project.
 */
class ASSETGATEEDITOR_API SAssetGateQualityConsole : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAssetGateQualityConsole)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
