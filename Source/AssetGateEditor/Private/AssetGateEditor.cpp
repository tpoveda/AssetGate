// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "AssetGateEditor.h"

#include "AssetGateCommands.h"
#include "SAssetGateQualityConsole.h"

#include "ToolMenus.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Commands/UICommandList.h"

#define LOCTEXT_NAMESPACE "FAssetGateEditorModule"

DEFINE_LOG_CATEGORY_STATIC(LogAssetGateEditor, Log, All)

void FAssetGateEditorModule::StartupModule()
{
	BindCommands();

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		                        AssetGateEditor::AssetQualityConsoleTabId,
		                        FOnSpawnTab::CreateRaw(this, &FAssetGateEditorModule::SpawnAssetQualityConsoleTab))
	                        .SetDisplayName(LOCTEXT("AssetGateAssetQualityConsole", "Asset Quality Console"))
	                        .SetMenuType(ETabSpawnerMenuType::Hidden);

	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(
			this, &FAssetGateEditorModule::RegisterMenus));

	UE_LOG(LogAssetGateEditor, Log, TEXT("AssetGateEditor module started."));
}

void FAssetGateEditorModule::ShutdownModule()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(AssetGateEditor::AssetQualityConsoleTabId);

	AssetGateCommands->Unregister();
	AssetGateCommands.Reset();
	AssetGateCommandList.Reset();

	UE_LOG(LogAssetGateEditor, Log, TEXT("AssetGateEditor module stopped."));
}

void FAssetGateEditorModule::BindCommands()
{
	AssetGateCommands = MakeShared<FAssetGateCommands>();
	AssetGateCommands->RegisterCommands();

	AssetGateCommandList = MakeShared<FUICommandList>();
	AssetGateCommandList->MapAction(
		AssetGateCommands->OpenAssetQualityConsole,
		FUIAction(
			FExecuteAction::CreateStatic(&FAssetGateEditorModule::OpenAssetQualityConsole),
			FCanExecuteAction()));
}

void FAssetGateEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.MainMenu.Window"));
	if (Menu == nullptr)
	{
		return;
	}

	FToolMenuSection& Section = Menu->FindOrAddSection(TEXT("AssetGate2"));
	Section.AddMenuEntry(
		TEXT("OpenAssetQualityConsole"),
		LOCTEXT("OpenAssetQualityConsoleMenuItem", "Asset Quality Console"),
		LOCTEXT("OpenAssetQualityConsoleMenuItemTooltip", "Open the AssetGate2 validation and quality console."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateStatic(&FAssetGateEditorModule::OpenAssetQualityConsole)));
}

void FAssetGateEditorModule::UnregisterMenus()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
}

TSharedRef<SDockTab> FAssetGateEditorModule::SpawnAssetQualityConsoleTab(const FSpawnTabArgs& Args)
{
	TSharedRef<SDockTab> NewTab = SNew(SDockTab)
		.TabRole(NomadTab)
		.Label(LOCTEXT("AssetQualityConsoleTabPanel", "Asset Quality Console"))
		[
			SNew(SAssetGateQualityConsole)
		];

	return NewTab;
}

void FAssetGateEditorModule::OpenAssetQualityConsole()
{
	FGlobalTabmanager::Get()->TryInvokeTab(AssetGateEditor::AssetQualityConsoleTabId);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAssetGateEditorModule, AssetGateEditor)
