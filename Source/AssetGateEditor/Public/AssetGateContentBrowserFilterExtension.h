// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ContentBrowserFrontEndFilterExtension.h"
#include "AssetGateContentBrowserFilterExtension.generated.h"

/**
 * Register AssetGate-specific frontend filters with the Content Browser.
 */
UCLASS()
class ASSETGATEEDITOR_API UAssetGateContentBrowserFilterExtension : public UContentBrowserFrontEndFilterExtension
{
	GENERATED_BODY()

public:
	/**
		 * Add the AssetGate browser filters to the Content Browser filter list.
		 *
		 * @param DefaultCategory Default category provided by the Content Browser.
		 * @param InOutFilterList The filter the list that receives AssetGate2 filters.
		 */
	virtual void AddFrontEndFilterExtensions(
		TSharedPtr<FFrontendFilterCategory> DefaultCategory,
		TArray<TSharedRef<FFrontendFilter>>& InOutFilterList) const override;
};
