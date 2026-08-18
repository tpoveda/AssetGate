// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "AssetGateCore.h"

#define LOCTEXT_NAMESPACE "FAssetGateCoreModule"

DEFINE_LOG_CATEGORY_STATIC(LogAssetGateCore, Log, All)

void FAssetGateCoreModule::StartupModule()
{
	UE_LOG(LogAssetGateCore, Log, TEXT("AssetGateCore module started."));
}

void FAssetGateCoreModule::ShutdownModule()
{
	UE_LOG(LogAssetGateCore, Log, TEXT("AssetGateCore module stopped."));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAssetGateCoreModule, AssetGateCore)
