// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "AssetGateCommands.h"

#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "FAssetGateCommands"

FAssetGateCommands::FAssetGateCommands() :
	TCommands<FAssetGateCommands>(
		TEXT("AssetGate"),
		LOCTEXT("AssetGate", "AssetGate"), NAME_None,
		FAppStyle::GetAppStyleSetName())
{
}

void FAssetGateCommands::RegisterCommands()
{
	UI_COMMAND(
		OpenAssetQualityConsole,
		"OpenAssetQualityConsole",
		"Open Asset Quality Console",
		EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
