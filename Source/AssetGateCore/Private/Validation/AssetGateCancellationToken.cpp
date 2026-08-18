// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.


#include "Validation/AssetGateCancellationToken.h"

// -------------------------------------
// FAssetGateCancellationToken
// -------------------------------------

FAssetGateCancellationToken::FAssetGateCancellationToken() :
	CancelledFlag(MakeShared<FThreadSafeBool, ESPMode::ThreadSafe>(false))
{
}

FAssetGateCancellationToken::FAssetGateCancellationToken(
	TSharedRef<FThreadSafeBool, ESPMode::ThreadSafe> InCancelledFlag) :
	CancelledFlag(MoveTemp(InCancelledFlag))
{
}


bool FAssetGateCancellationToken::IsCancellationRequested() const
{
	return *CancelledFlag;
}


// -------------------------------------
// FAssetGateCancellationTokenSource
// -------------------------------------

FAssetGateCancellationTokenSource::FAssetGateCancellationTokenSource() :
	CancelledFlag(MakeShared<FThreadSafeBool, ESPMode::ThreadSafe>(false))
{
}

void FAssetGateCancellationTokenSource::RequestCancel()
{
	*CancelledFlag = true;
}

bool FAssetGateCancellationTokenSource::IsCancellationRequested() const
{
	return *CancelledFlag;
}

FAssetGateCancellationToken FAssetGateCancellationTokenSource::GetToken() const
{
	return FAssetGateCancellationToken(CancelledFlag);
}
