// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

/**
 * Command definitions for AssetGate editor actions.
 */
class ASSETGATEEDITOR_API FAssetGateCommands : public TCommands<FAssetGateCommands>
{
public:
	FAssetGateCommands();

	/** Registers the command metadata used by the editor UI. */
	virtual void RegisterCommands() override;

	/** Command used to open the Asset Quality Console. */
	TSharedPtr<FUICommandInfo> OpenAssetQualityConsole;
};
