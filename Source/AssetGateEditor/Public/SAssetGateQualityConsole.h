// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPath.h"
#include "Widgets/SCompoundWidget.h"

template <typename ItemType>
class SListView;

/**
 * Row model used by the Asset Quality Console result list.
 */
struct FAssetGateConsoleRow
{
	/** Asset-friendly name displayed in the list view. */
	FString AssetName;

	/** Rule identifier that produced the diagnostic. */
	FString RuleName;

	/** Human-readable severity label displayed in editor UI. */
	FString Severity;

	/** Human-readable reason text shown in the result list. */
	FString Message;

	/** Full asset path used for navigation and inspection actions. */
	FSoftObjectPath AssetPath;
};

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

	/**
	 * Construct the widget and populates the initial results grid.
	 *
	 * @param InArgs Slate construction arguments.
	 */
	void Construct(const FArguments& InArgs);

private:
	/** Refresh the in-memory row list from the diagnostic transport. */
	void RefreshRows();

	/** Cached result rows rendered by the list view. */
	TArray<TSharedPtr<FAssetGateConsoleRow>> Rows;

	/** Reference to the list widget used to display diagnostics. */
	TSharedPtr<SListView<TSharedPtr<FAssetGateConsoleRow>>> ListView;
};
