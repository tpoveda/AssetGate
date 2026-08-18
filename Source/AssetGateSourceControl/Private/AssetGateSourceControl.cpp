// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "AssetGateSourceControl.h"

#define LOCTEXT_NAMESPACE "FAssetGateSourceControlModule"

DEFINE_LOG_CATEGORY_STATIC(LogAssetGateSourceControl, Log, All);

void FAssetGateSourceControlModule::StartupModule()
{
	UE_LOG(LogAssetGateSourceControl, Log, TEXT("AssetGateSourceControl module started."));
}

void FAssetGateSourceControlModule::ShutdownModule()
{
	UE_LOG(LogAssetGateSourceControl, Log, TEXT("AssetGateSourceControl module started."));

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAssetGateSourceControlModule, AssetGateSourceControl)
