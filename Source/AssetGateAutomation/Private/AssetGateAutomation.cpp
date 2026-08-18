// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "AssetGateAutomation.h"

#define LOCTEXT_NAMESPACE "FAssetGateAutomationModule"

DEFINE_LOG_CATEGORY_STATIC(LogAssetGateAutomation, Log, All)

void FAssetGateAutomationModule::StartupModule()
{
	UE_LOG(LogAssetGateAutomation, Log, TEXT("AssetGateAutomation module started."))
}

void FAssetGateAutomationModule::ShutdownModule()
{
	UE_LOG(LogAssetGateAutomation, Log, TEXT("AssetGateAutomation module stopped."))
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAssetGateAutomationModule, AssetGateAutomation)
