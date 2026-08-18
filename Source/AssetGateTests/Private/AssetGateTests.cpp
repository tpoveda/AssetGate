// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "AssetGateTests.h"

#define LOCTEXT_NAMESPACE "FAssetGateTestsModule"

DEFINE_LOG_CATEGORY_STATIC(LogAssetGateTests, Log, All);

void FAssetGateTestsModule::StartupModule()
{
	UE_LOG(LogAssetGateTests, Log, TEXT("AssetGateTests module started."));
}

void FAssetGateTestsModule::ShutdownModule()
{
	UE_LOG(LogAssetGateTests, Log, TEXT("AssetGateTests module stopped."));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAssetGateTestsModule, AssetGateTests)
