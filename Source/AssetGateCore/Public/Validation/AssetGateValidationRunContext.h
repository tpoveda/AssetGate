// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetGateCancellationToken.h"

/**
 * Identifies what triggered a validation run.
 * 
 * Downstream systems (policy gates, reporting, CI) use this to select thresholds and
 * behavior appropriate to the run's trigger, independent of any single validator's own
 * logic.
 * 
 * This is an AssetGate own concept and is separate from Unreal's EDataValidationUseCase; 
 * a mapping between the two is introduced where AssetGate talks to the Data Validation
 * subsystem, not here.
 */
enum class EAssetGateValidationRunMode : uint8
{
	/** Explicitly triggered by a user from the editor (Content Browser action, menu, command). */
	Manual,

	/** Triggered by a Perforce pre-submit hook. */
	PreSubmit,

	/** Triggered by a Git pre-commit hook. */
	PreCommit,

	/** Triggered by a continuous integration pipeline. */
	CI
};

struct ASSETGATECORE_API FAssetGateValidationScope
{
	/** When true, a run using this scope should evaluate only ChangedAssetPaths instead
	 * of a full project sweep.
	 */
	bool bChangedAssetsOnly{ false };

	/** An explicit set of asset paths considered "changed" for this run. Ignored unless bChangedAssetsOnly is true. */
	TArray<FSoftObjectPath> ChangedAssetPaths;

	/** @return a scope covering the entire project (default validation sweep). */
	static FAssetGateValidationScope MakeFullProjectScope();

	/** @return a scope restricted to the given changed asset paths. */
	static FAssetGateValidationScope MakeChangedAssetsScope(TArray<FSoftObjectPath> InChangedAssetPaths);

	/** @return true when this scope includes InAssetPath. */
	bool Contains(const FSoftObjectPath& InAssetPath) const;
};

/**
 * Carries the run-level inputs shared by every validator/diagnostic produced during one
 * AssetGate validation pass: what triggered it, which assets it covers, and how to
 * observe a cancellation request.
 * 
 * This is distinct from Unreal's per-asset FDataValidationContext. FDataValidationContext
 * is UE's message-collection object for a single asset's validation call; this struct is
 * AssetGate's own end-to-end run description that spans the whole batch (a manual run, a
 * pre-submit hook, a CI job, ...) and is expected to be threaded through the batch
 * executor and policy gates.
 */

struct ASSETGATECORE_API FAssetGateValidationRunMetadata
{
	/** Human-readable label for the selected scope. */
	FString ScopeLabel{ TEXT("Full Project") };

	/** Human-readable label for the triggering run mode. */
	FString RunModeLabel{ TEXT("Manual") };

	/** The number of assets the run was created to evaluate. */
	int32 AssetCount{ 0 };

	/** Elapsed wall-clock time for the run in seconds. */
	double ElapsedSeconds{ 0 };

	/** Refreshes UI metadata using the current context values. */
	void Refresh(
		const EAssetGateValidationRunMode InRunMode,
		const FAssetGateValidationScope& InScope,
		int32 InAssetCount = 0,
		double InElapsedSeconds = 0.0f);
};

struct ASSETGATECORE_API FAssetGateValidationRunContext
{
	/** What triggered this validation run. */
	EAssetGateValidationRunMode RunMode{ EAssetGateValidationRunMode::Manual };

	/** Which assets this run is expected to cover. */
	FAssetGateValidationScope Scope;

	/** UI-friendly metadata emitted for editor, browser, and reporting surfaces. */
	FAssetGateValidationRunMetadata Metadata;

	/** Cooperative cancellation signal validators/batch executors should poll between assets. */
	FAssetGateCancellationToken CancellationToken;

	/** @return true when the caller has requested this run stop early. */
	bool IsCancellationRequested() const;

	/** Refresh the UI metadata derived from the current run state. */
	void RefreshMetadata();

	/** Sets runtime information that the editor UI can surface while the run is active. */
	void SetRuntimeMetadata(int32 InAssetCount, double InElapsedSeconds = 0.0f);

	/** Convenience constructor for a manual, full-project run with no cancellation source. */
	static FAssetGateValidationRunContext MakeManualFullProjectRun();
};
