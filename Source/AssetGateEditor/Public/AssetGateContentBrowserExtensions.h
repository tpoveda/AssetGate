// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace AssetGate
{
	/** Register custom extensions for the Content Browser within the AssetGate namespace. */
	void RegisterContentBrowserExtensions();

	/** Unregister previously registered custom extensions for the Content Browser within the AssetGate namespace. */
	void UnregisterContentBrowserExtensions();

	/* Register the AssetGate Content Browser filter extension. */
	void RegisterContentBrowserFilter();
	
	/** Register invalid-asset issue indicators in the Content Browser asset view. */
	void RegisterAssetIssueIndicators();

	/** Unregister invalid-asset issue indicators from the Content Browser asset view. */
	void UnregisterAssetIssueIndicators();
}
