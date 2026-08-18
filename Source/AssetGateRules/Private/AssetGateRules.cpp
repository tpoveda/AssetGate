// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "AssetGateRules.h"

#define LOCTEXT_NAMESPACE "FAssetGateRulesModule"

DEFINE_LOG_CATEGORY_STATIC(LogAssetGateRules, Log, All)

void FAssetGateRulesModule::StartupModule()
{
	UE_LOG(LogAssetGateRules, Log, TEXT("AssetGateRules module started."))
}

void FAssetGateRulesModule::ShutdownModule()
{
	UE_LOG(LogAssetGateRules, Log, TEXT("AssetGateRules module stopped."))
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAssetGateRulesModule, AssetGateRules)
