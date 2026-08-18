// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace AssetGate
{
	/** Register custom extensions for the Content Browser within the AssetGate namespace. */
	void RegisterContentBrowserExtensions();

	/** Unregister previously registered custom extensions for the Content Browser within the AssetGate namespace. */
	void UnregisterContentBrowserExtensions();
}
