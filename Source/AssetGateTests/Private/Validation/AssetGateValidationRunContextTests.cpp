// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "Validation/AssetGateCancellationToken.h"
#include "Validation/AssetGateValidationRunContext.h"

#include "Misc/AutomationTest.h"
#include "UObject/SoftObjectPath.h"

#if WITH_EDITOR

/**
 * Run mode defaults, the changed-assets-only scope contract, and cancellation token
 * propagation from a token source into a context that only holds a read-only token.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAssetGateValidationRunContextTest,
	"AssetGate.Core.Validation.ValidationRunContext",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FAssetGateValidationRunContextTest::RunTest(const FString& Parameters)
{
	// A full-project scope should consider every asset path in scope.
	{
		const FAssetGateValidationScope FullProjectScope = FAssetGateValidationScope::MakeFullProjectScope();
		TestFalse(TEXT("Full project scope should not be changed-assets-only"), FullProjectScope.bChangedAssetsOnly);
		TestTrue(TEXT("Full project scope should contain an arbitrary asset path"),
		         FullProjectScope.Contains(FSoftObjectPath(TEXT("/Game/Arbitrary/Asset.Asset"))));
	}

	// A changed-assets-only scope should only contain the paths it was given.
	{
		const FSoftObjectPath ChangedAsset(TEXT("/Game/Changed/Asset.Asset"));
		const FSoftObjectPath UnrelatedAsset(TEXT("/Game/Unrelated/Asset.Asset"));

		const FAssetGateValidationScope ChangedScope = FAssetGateValidationScope::MakeChangedAssetsScope(
			{ ChangedAsset });
		TestTrue(TEXT("Changed-assets-only scope should be flagged as such"), ChangedScope.bChangedAssetsOnly);
		TestTrue(TEXT("Changed-assets-only scope should contain a listed asset"), ChangedScope.Contains(ChangedAsset));
		TestFalse(
			TEXT("Changed-assets-only scope should not contain an unlisted asset"),
			ChangedScope.Contains(UnrelatedAsset));
	}

	// The convenience manual run constructor should default to Manual + full project scope.
	{
		const FAssetGateValidationRunContext ManualRun = FAssetGateValidationRunContext::MakeManualFullProjectRun();
		TestEqual(TEXT("Manual convenience run should use Manual run mode"),
		          ManualRun.RunMode, EAssetGateValidationRunMode::Manual);
		TestFalse(TEXT("Manual convenience run should not be changed-assets-only"), ManualRun.Scope.bChangedAssetsOnly);
		TestFalse(TEXT("Manual convenience run should not start cancelled"), ManualRun.IsCancellationRequested());
	}

	// A cancellation request made on the source should be observable through a context
	// that only holds the read-only token, without the context needing the source itself.
	{
		FAssetGateCancellationTokenSource CancellationSource;

		FAssetGateValidationRunContext Context;
		Context.RunMode = EAssetGateValidationRunMode::CI;
		Context.CancellationToken = CancellationSource.GetToken();

		TestFalse(
			TEXT("Context should not report cancellation before it is requested"),
			Context.IsCancellationRequested());

		CancellationSource.RequestCancel();

		TestTrue(
			TEXT("Context should observe cancellation requested on its source"),
			Context.IsCancellationRequested());
	}

	return true;
}

#endif
