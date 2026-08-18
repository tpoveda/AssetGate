// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FFrontendFilter;
struct FAssetData;
class FAssetGateCommands;
class FUICommandInfo;
class FUICommandList;
class FExtender;
class FSpawnTabArgs;
class SDockTab;

namespace AssetGateEditor
{
	static const FName AssetQualityConsoleTabId = TEXT("AssetGateAssetQualityConsole");
	static const FName HasIssuesFilterName = TEXT("AssetGateHasIssues");
}

/** 
 * Module entry point for AssetGate editor integrations.
 */
class FAssetGateEditorModule : public IModuleInterface
{
public:
	/**
	 * Retrieve the singleton instance of the Asset Gate editor module.
	 * 
	 * @return A reference to the singleton instance of FAssetGateEditorModule.
	 */
	static FAssetGateEditorModule& Get();

	/**
	 * Determine if the Asset Gate Editor module is currently available.
	 *
	 * @return True if the Asset Gate Editor module is available; false otherwise.
	 */
	static bool IsAvailable();

	//~ Begin IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface

	/*Emits a placeholder validation request for the current asset selection. */
	void ValidateSelectedAssets();

	/* Emits a placeholder recursive validation request for the current folder selection. */
	void ValidateSelectedPaths();

	/**
	 * Returns true when the selected asset has one or more recorded AssetGate diagnostics.
	 *
	 * @param AssetPath The asset path being queried by the Content Browser surfaces.
	 * @return True when the asset has recorded issues.
	 */
	static bool AssetHasRecordedIssues(const FSoftObjectPath& AssetPath);

	/**
	 * Returns true when the supplied Content Browser item path matches a recorded AssetGate diagnostic.
	 *
	 * @param ItemPath The invariant Content Browser path for the asset item.
	 * @return True when the item has recorded issues.
	 */
	static bool AssetHasRecordedIssues(const FName ItemPath);

private:
	/** Register the tab spawner for the Asset Gate Editor. */
	void RegisterTabSpawner();

	/** Unregister the tab spawner for the Asset Gate Editor. */
	void UnregisterTabSpawner();

	/** Registers the Level Editor Window menu entry. */
	void RegisterMenus();

	/** Unregister the menus registered during StartupModule(). */
	void UnregisterMenus();

	/**
	 * Spawn and return a new dockable tab for the Asset Quality Console.
	 *
	 * @param Args The arguments used for spawning the tab.
	 * @return A shared reference to the newly created dock tab for the Asset Quality Console.
	 */
	TSharedRef<SDockTab> SpawnAssetQualityConsoleTab(const FSpawnTabArgs& Args);

	/** Open the Asset Quality Console for inspection and debugging purposes. */
	static void OpenAssetQualityConsole();

	/** Execute a lightweight validation refresh action from the editor menu or toolbar entry points. */
	void RunQuickValidation();

	/** Command binding for the Asset Quality Console open action. */
	TSharedPtr<const FUICommandInfo> OpenAssetQualityConsoleCommand;

	/** Cached asset selection used by the current menu extension callback. */
	TArray<FAssetData> SelectedAssetContext;

	/** Cached path selection used by the current menu extension callback. */
	TArray<FString> SelectedPathContext;
};
