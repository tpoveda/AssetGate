// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "AssetGateEditor.h"

#include "AssetGateCommands.h"
#include "AssetGateContentBrowserExtensions.h"
#include "SAssetGateValidationConsole.h"

#include "ToolMenus.h"
#include "LevelEditor.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Logging/MessageLog.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Application/SlateApplication.h"
#include "Validation/AssetGateDiagnosticTransport.h"
#include "Validation/AssetGateValidationTypes.h"

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
	AssetGate::RegisterContentBrowserFilter();
	AssetGate::RegisterAssetIssueIndicators();

	UE_LOG(LogAssetGateEditor, Log, TEXT("AssetGateEditor module started."));
}

void FAssetGateEditorModule::ShutdownModule()
{
	AssetGate::UnregisterAssetIssueIndicators();
	AssetGate::UnregisterContentBrowserFilter();
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

bool FAssetGateEditorModule::AssetHasRecordedIssues(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsNull())
	{
		return false;
	}

	for (const FAssetGateDiagnostic& Diagnostic : FAssetGateDiagnosticTransport::Get().GetRecentDiagnostics())
	{
		if (Diagnostic.AssetPath == AssetPath)
		{
			return true;
		}
	}

	return false;
}

bool FAssetGateEditorModule::AssetHasRecordedIssues(const FName ItemPath)
{
	if (ItemPath.IsNone())
	{
		return false;
	}

	const FString ItemPathString = ItemPath.ToString();
	for (const FAssetGateDiagnostic& Diagnostic : FAssetGateDiagnosticTransport::Get().GetRecentDiagnostics())
	{
		if (Diagnostic.AssetPath.IsNull())
		{
			continue;
		}

		if (Diagnostic.AssetPath.GetAssetPathString() == ItemPathString)
		{
			return true;
		}
	}

	return false;
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

	FToolMenuSection& Section = Menu->FindOrAddSection(TEXT("AssetGate"));
	Section.AddMenuEntry(
		TEXT("OpenAssetQualityConsole"),
		LOCTEXT("OpenAssetQualityConsoleMenuItem", "Asset Quality Console"),
		LOCTEXT("OpenAssetQualityConsoleMenuItemTooltip", "Open the AssetGate validation and quality console."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateStatic(&FAssetGateEditorModule::OpenAssetQualityConsole)));

	UToolMenu* Toolbar = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.LevelEditorToolBar.PlayToolBar"));
	if (Toolbar)
	{
		FToolMenuSection& ToolbarSection = Toolbar->FindOrAddSection(TEXT("AssetGate"));
		ToolbarSection.AddMenuEntry(
			TEXT("OpenAssetQualityConsoleToolbar"),
			LOCTEXT("OpenAssetQualityConsoleToolbarLabel", "Asset Quality"),
			LOCTEXT("OpenAssetQualityConsoleToolbarTooltip", "Open the AssetGate Asset Quality Console."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateStatic(&FAssetGateEditorModule::OpenAssetQualityConsole)));
		ToolbarSection.AddMenuEntry(
			TEXT("RunQuickAssetGateValidationToolbar"),
			LOCTEXT("RunQuickAssetGateValidationLabel", "Validate"),
			LOCTEXT("RunQuickAssetGateValidationTooltip",
			        "Trigger the current AssetGate validation workflow placeholder."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FAssetGateEditorModule::RunQuickValidation)));
	}
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
			SNew(SAssetGateValidationConsole)
		];

	return NewTab;
}

void FAssetGateEditorModule::OpenAssetQualityConsole()
{
	FGlobalTabmanager::Get()->TryInvokeTab(AssetGateEditor::AssetQualityConsoleTabId);
}

void FAssetGateEditorModule::RunQuickValidation()
{
	FMessageLog AssetGateLog("AssetGate");
	AssetGateLog.Info()->AddToken(FTextToken::Create(
		LOCTEXT(
			"RunQuickValidationPlaceholder",
			"AssetGate quick validation entry point is wired and ready for the next validation execution slice.")));
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAssetGateEditorModule, AssetGateEditor)
