// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "AssetGateEditor.h"

#define LOCTEXT_NAMESPACE "FAssetGateEditorModule"

DEFINE_LOG_CATEGORY_STATIC(LogAssetGateEditor, Log, All)

void FAssetGateEditorModule::StartupModule()
{
	UE_LOG(LogAssetGateEditor, Log, TEXT("AssetGateEditor module started."));
}

void FAssetGateEditorModule::ShutdownModule()
{
	UE_LOG(LogAssetGateEditor, Log, TEXT("AssetGateEditor module stopped."));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAssetGateEditorModule, AssetGateEditor)
