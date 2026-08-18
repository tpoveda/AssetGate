// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.


#include "Validation/AssetGateValidationRunContext.h"

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

bool FAssetGateValidationRunContext::IsCancellationRequested() const
{
	return CancellationToken.IsCancellationRequested();
}

FAssetGateValidationRunContext FAssetGateValidationRunContext::MakeManualFullProjectRun()
{
	FAssetGateValidationRunContext Context;
	Context.RunMode = EAssetGateValidationRunMode::Manual;
	Context.Scope = FAssetGateValidationScope::MakeFullProjectScope();
	return Context;
}
