// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

struct FAssetData;
class FAssetGateCommands;
class FUICommandInfo;
class FUICommandList;
class FExtender;
class FSpawnTabArgs;
class SDockTab;

namespace AssetGateEditor
{
	static const FName AssetQualityConsoleTabId = TEXT("AssetGate2AssetQualityConsole");
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

	/** Command binding for the Asset Quality Console open action. */
	TSharedPtr<const FUICommandInfo> OpenAssetQualityConsoleCommand;

	/** Cached asset selection used by the current menu extension callback. */
	TArray<FAssetData> SelectedAssetContext;

	/** Cached path selection used by the current menu extension callback. */
	TArray<FString> SelectedPathContext;
};
