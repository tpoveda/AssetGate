// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "AssetGateEditor.h"

#include "AssetGateCommands.h"
#include "AssetGateContentBrowserExtensions.h"
#include "SAssetGateQualityConsole.h"

#include "ToolMenus.h"
#include "LevelEditor.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Logging/MessageLog.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Application/SlateApplication.h"

#define LOCTEXT_NAMESPACE "FAssetGateEditorModule"

DEFINE_LOG_CATEGORY_STATIC(LogAssetGateEditor, Log, All)

FAssetGateEditorModule& FAssetGateEditorModule::Get()
{
	return FModuleManager::LoadModuleChecked<FAssetGateEditorModule>("AssetGateEditor");
}

bool FAssetGateEditorModule::IsAvailable()
{
	return FModuleManager::Get().IsModuleLoaded("AssetGateEditor");
}

void FAssetGateEditorModule::StartupModule()
{
	FAssetGateCommands::Register();

	RegisterTabSpawner();

	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(
			this, &FAssetGateEditorModule::RegisterMenus));

	AssetGate::RegisterContentBrowserExtensions();

	UE_LOG(LogAssetGateEditor, Log, TEXT("AssetGateEditor module started."));
}

void FAssetGateEditorModule::ShutdownModule()
{
	AssetGate::UnregisterContentBrowserExtensions();

	UnregisterMenus();

	UnregisterTabSpawner();

	FAssetGateCommands::Unregister();

	SelectedAssetContext.Reset();
	SelectedPathContext.Reset();

	UE_LOG(LogAssetGateEditor, Log, TEXT("AssetGateEditor module stopped."));
}

void FAssetGateEditorModule::ValidateSelectedAssets()
{
	FMessageLog AssetGateLog("AssetGate");
	AssetGateLog.Info()->AddToken(FTextToken::Create(FText::Format(
		LOCTEXT("ValidateSelectedAssetsPlaceholder",
		        "AssetGate browser validation is wired for {0} selected asset(s)."),
		FText::AsNumber(SelectedAssetContext.Num()))));
}

void FAssetGateEditorModule::ValidateSelectedPaths()
{
	FMessageLog AssetGateLog("AssetGate");
	AssetGateLog.Info()->AddToken(FTextToken::Create(FText::Format(
		LOCTEXT("ValidateSelectedPathsPlaceholder",
		        "AssetGate folder validation is wired for {0} selected folder(s)."),
		FText::AsNumber(SelectedPathContext.Num()))));
}

void FAssetGateEditorModule::RegisterTabSpawner()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		                        AssetGateEditor::AssetQualityConsoleTabId,
		                        FOnSpawnTab::CreateRaw(this, &FAssetGateEditorModule::SpawnAssetQualityConsoleTab))
	                        .SetDisplayName(LOCTEXT("AssetGateAssetQualityConsole", "Asset Quality Console"))
	                        .SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());

}

void FAssetGateEditorModule::UnregisterTabSpawner()
{
	if (FSlateApplication::IsInitialized())
	{
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(AssetGateEditor::AssetQualityConsoleTabId);
	}
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

	if (UObjectInitialized() && !IsEngineExitRequested())
	{
		UToolMenus::UnregisterOwner(this);
	}
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
