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
	//~ Begin IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface

private:
	/** Binds commands to the AssetGate editor module. */
	void BindCommands();

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

	/** Shared command set for editor actions. */
	TSharedPtr<FAssetGateCommands> AssetGateCommands;

	/** Command list used for defining and binding AssetGate-specific editor commands. */
	TSharedPtr<FUICommandList> AssetGateCommandList;

	/** Command binding for the Asset Quality Console open action. */
	TSharedPtr<const FUICommandInfo> OpenAssetQualityConsoleCommand;
};
