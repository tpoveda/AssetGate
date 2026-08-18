// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "AssetGateContentBrowserFilterExtension.h"

#include "AssetGateEditor.h"

namespace
{
	/**
	 * Frontend filter that shows only assets with recorded AssetGate diagnostics.
	 */
	class FAssetGateHasIssuesFilter : public FFrontendFilter
	{
	public:
		explicit FAssetGateHasIssuesFilter(TSharedPtr<FFrontendFilterCategory> InCategory) :
			FFrontendFilter(InCategory)
		{
		}

		virtual FString GetName() const override
		{
			return TEXT("AssetGateHasIssues");
		}

		virtual FText GetDisplayName() const override
		{
			return NSLOCTEXT("AssetGate", "HasIssuesFilterName", "Has Asset Gate issues");
		}

		virtual FText GetToolTipText() const override
		{
			return NSLOCTEXT(
				"AssetGate", "HasIssuesFilterTooltip",
				"Shows only assets with recorded AssetGate2 diagnostics.");
		}

		virtual FLinearColor GetColor() const override
		{
			return FLinearColor(0.85f, 0.25f, 0.2f);
		}

		virtual bool PassesFilter(FAssetFilterType InItem) const override
		{
			const FName InvariantPath = InItem.GetInvariantPath();
			if (InvariantPath.IsNone())
			{
				return false;
			}

			return FAssetGateEditorModule::AssetHasRecordedIssues(InvariantPath);
		}
	};
}

void UAssetGateContentBrowserFilterExtension::AddFrontEndFilterExtensions(
	const TSharedPtr<FFrontendFilterCategory> DefaultCategory,
	TArray<TSharedRef<FFrontendFilter>>& InOutFilterList) const
{
	TSharedPtr<FFrontendFilterCategory> Category = DefaultCategory.IsValid()
		? DefaultCategory
		: MakeShared<FFrontendFilterCategory>(
			NSLOCTEXT("AssetGate", "AssetGateFilterCategory", "AssetGate"),
			NSLOCTEXT("AssetGate", "AssetGateFilterCategoryTooltip", "AssetGate browser filters."));

	InOutFilterList.Add(MakeShared<FAssetGateHasIssuesFilter>(Category));
}
