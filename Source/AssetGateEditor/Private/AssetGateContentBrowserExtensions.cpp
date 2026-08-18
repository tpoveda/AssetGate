// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.


#include "AssetGateContentBrowserExtensions.h"

#include "AssetGateEditor.h"
#include "ContentBrowserModule.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

#define LOCTEXT_NAMESPACE "AssetGateContentBrowserExtensions"

namespace AssetGate::ContentBrowserPrivate
{
	static FDelegateHandle AssetContextMenuHandle;
	static FDelegateHandle PathContextMenuHandle;

	/**
	 * Extends the asset context menu in the Content Browser by adding custom options based on the selected assets.
	 *
	 * @param SelectedAssets An array of FAssetData objects representing the currently selected assets in the Content Browser.
	 * @return A shared reference to an FExtender object that defines the custom menu extensions.
	 */
	static TSharedRef<FExtender> ExtendAssetContextMenu(const TArray<FAssetData>& SelectedAssets)
	{
		const TSharedRef<FExtender> Extender = MakeShared<FExtender>();

		if (SelectedAssets.IsEmpty())
		{
			return Extender;
		}

		Extender->AddMenuExtension(
			"GetAssetActions",
			EExtensionHook::After,
			nullptr,
			FMenuExtensionDelegate::CreateLambda([](FMenuBuilder& MenuBuilder)
			{
				MenuBuilder.BeginSection(TEXT("AssetGate"), LOCTEXT("AssetGateAssetSection", "Asset Gate"));
				MenuBuilder.AddMenuEntry(
					LOCTEXT("ValidateWithAssetGate", "Validate with AssetGate"),
					LOCTEXT("ValidateWithAssetGateTooltip", "Validate the selected assets with AssetGate."),
					FSlateIcon(),
					FUIAction(FExecuteAction::CreateLambda([]
					{
						if (FAssetGateEditorModule::IsAvailable())
						{
							FAssetGateEditorModule::Get().ValidateSelectedAssets();
						}
					})));
				MenuBuilder.EndSection();
			}));

		return Extender;
	}

	static TSharedRef<FExtender> ExtendPathContextMenu(const TArray<FString>& Paths)
	{
		const TSharedRef<FExtender> Extender = MakeShared<FExtender>();

		if (Paths.IsEmpty())
		{
			return Extender;
		}

		Extender->AddMenuExtension(
			"PathContextBulkOperations",
			EExtensionHook::After,
			nullptr,
			FMenuExtensionDelegate::CreateLambda([](FMenuBuilder& MenuBuilder)
			{
				MenuBuilder.BeginSection(TEXT("Asset Gate"), LOCTEXT("AssetGatePathSection", "Asset Gate"));
				MenuBuilder.AddMenuEntry(
					LOCTEXT("ValidateFolderRecursively", "Validate Folder Recursively"),
					LOCTEXT("ValidateFolderRecursivelyTooltip",
					        "Run AssetGate validation recursively for the selected folders."),
					FSlateIcon(),
					FUIAction(FExecuteAction::CreateLambda([]
					{
						if (FAssetGateEditorModule::IsAvailable())
						{
							FAssetGateEditorModule::Get().ValidateSelectedPaths();
						}
					})));
				MenuBuilder.EndSection();
			}));
		return Extender;

		return Extender;
	}
}


void AssetGate::RegisterContentBrowserExtensions()
{
	using namespace AssetGate::ContentBrowserPrivate;

	FContentBrowserModule& CBModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	auto AssetExtender = FContentBrowserMenuExtender_SelectedAssets::CreateStatic(&ExtendAssetContextMenu);
	AssetContextMenuHandle = AssetExtender.GetHandle();
	CBModule.GetAllAssetViewContextMenuExtenders().Add(MoveTemp(AssetExtender));

	auto PathExtender = FContentBrowserMenuExtender_SelectedPaths::CreateStatic(&ExtendPathContextMenu);
	PathContextMenuHandle = AssetExtender.GetHandle();
	CBModule.GetAllPathViewContextMenuExtenders().Add(MoveTemp(PathExtender));
}

void AssetGate::UnregisterContentBrowserExtensions()
{
	using namespace AssetGate::ContentBrowserPrivate;

	FContentBrowserModule* CBModule = FModuleManager::GetModulePtr<FContentBrowserModule>(TEXT("ContentBrowser"));
	if (!CBModule)
	{
		return;
	}

	CBModule->GetAllAssetViewContextMenuExtenders().RemoveAll(
		[](const FContentBrowserMenuExtender_SelectedAssets& Extender)
		{
			return Extender.GetHandle() == AssetContextMenuHandle;
		});

	CBModule->GetAllPathViewContextMenuExtenders().RemoveAll(
		[](const FContentBrowserMenuExtender_SelectedPaths& Extender)
		{
			return Extender.GetHandle() == PathContextMenuHandle;
		});

	AssetContextMenuHandle.Reset();
	PathContextMenuHandle.Reset();
}
