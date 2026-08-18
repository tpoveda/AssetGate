// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.


#include "Validation/AssetGateValidationRunContext.h"

namespace
{
	/**
	 * Retrieves the label associated with a specific asset gate validation run mode.
	 *
	 * @param InRunMode The run mode for which to retrieve the corresponding label.
	 *                  The available modes are:
	 *                  - Manual: Triggered explicitly by a user from the editor.
	 *                  - PreSubmit: Triggered by a Perforce pre-submit hook.
	 *                  - PreCommit: Triggered by a Git pre-commit hook.
	 *                  - CI: Triggered by a continuous integration pipeline.
	 * @return A string representing the label of the specified run mode. If the run mode does not
	 *         match any known mode, "Unknown" is returned.
	 */
	FString GetRunModeLabel(EAssetGateValidationRunMode InRunMode)
	{
		switch (InRunMode)
		{
		case EAssetGateValidationRunMode::Manual:
			return TEXT("Manual");
		case EAssetGateValidationRunMode::PreSubmit:
			return TEXT("Pre-submit");
		case EAssetGateValidationRunMode::PreCommit:
			return TEXT("Pre-commit");
		case EAssetGateValidationRunMode::CI:
			return TEXT("CI");
		default:
			return TEXT("Unknown");
		}
	}

	/**
	 * Retrieves the label associated with a specific asset gate validation scope.
	 *
	 * @param InScope The validation scope for which to retrieve the label. This includes:
	 *                - bChangedAssetsOnly: Determines whether the scope includes only changed assets
	 *                  or the entire project.
	 *                - ChangedAssetPaths: A list of asset paths marked as "changed" for the scope.
	 * @return A string representing the label of the specified validation scope. Possible values are:
	 *         - "Full project" if the scope covers the entire project.
	 *         - "Changed assets (0)" if the scope is limited to changed assets, but no assets are flagged as changed.
	 *         - "Changed assets (N)" where N is the number of changed assets in the scope.
	 */
	FString GetScopeLabel(const FAssetGateValidationScope& InScope)
	{
		if (!InScope.bChangedAssetsOnly)
		{
			return TEXT("Full project");
		}

		if (InScope.ChangedAssetPaths.Num() == 0)
		{
			return TEXT("Changed assets (0)");
		}

		return FString::Printf(TEXT("Changed assets (%d)"), InScope.ChangedAssetPaths.Num());
	}
}

FAssetGateValidationScope FAssetGateValidationScope::MakeFullProjectScope()
{
	FAssetGateValidationScope Scope;
	Scope.bChangedAssetsOnly = false;
	return Scope;
}

FAssetGateValidationScope FAssetGateValidationScope::MakeChangedAssetsScope(TArray<FSoftObjectPath> InChangedAssetPaths)
{
	FAssetGateValidationScope Scope;
	Scope.bChangedAssetsOnly = true;
	Scope.ChangedAssetPaths = MoveTemp(InChangedAssetPaths);
	return Scope;
}

bool FAssetGateValidationScope::Contains(const FSoftObjectPath& InAssetPath) const
{
	if (!bChangedAssetsOnly)
	{
		return true;
	}

	return ChangedAssetPaths.Contains(InAssetPath);
}

void FAssetGateValidationRunMetadata::Refresh(
	const EAssetGateValidationRunMode InRunMode,
	const FAssetGateValidationScope& InScope,
	const int32 InAssetCount,
	const double InElapsedSeconds)
{
	ScopeLabel = GetScopeLabel(InScope);
	RunModeLabel = GetRunModeLabel(InRunMode);
	AssetCount = InAssetCount;
	ElapsedSeconds = InElapsedSeconds;
}

bool FAssetGateValidationRunContext::IsCancellationRequested() const
{
	return CancellationToken.IsCancellationRequested();
}

void FAssetGateValidationRunContext::RefreshMetadata()
{
	Metadata.Refresh(RunMode, Scope, Metadata.AssetCount, Metadata.ElapsedSeconds);
}

void FAssetGateValidationRunContext::SetRuntimeMetadata(const int32 InAssetCount, const double InElapsedSeconds)
{
	Metadata.AssetCount = InAssetCount;
	Metadata.ElapsedSeconds = InElapsedSeconds;
	Metadata.Refresh(RunMode, Scope, InAssetCount, InElapsedSeconds);
}

FAssetGateValidationRunContext FAssetGateValidationRunContext::MakeManualFullProjectRun()
{
	FAssetGateValidationRunContext Context;
	Context.RunMode = EAssetGateValidationRunMode::Manual;
	Context.Scope = FAssetGateValidationScope::MakeFullProjectScope();
	return Context;
}
