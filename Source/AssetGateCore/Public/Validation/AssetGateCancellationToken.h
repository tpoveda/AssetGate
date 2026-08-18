// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/ThreadSafeBool.h"

/**
 * Read-only handle to a validation run's cancellation signal.
 * 
 * Threaded through the validation context and polled cooperatively by validators and
 * batch executors; it can observe a cancellation request but cannot make one. A 
 * default-constructed token owns its own flag that is never set, so code that receives a 
 * token (rather tna constructing one) never needs to null-check it.
 */
class ASSETGATECORE_API FAssetGateCancellationToken
{
public:
	/** Construct a token that can never ben canceled. */
	FAssetGateCancellationToken();

	/** @return true once the owning FAssetGateCancellationTokenSource has requested cancellation. */
	bool IsCancellationRequested() const;

	/** Allow FAssetGateCancellationTokenSource to access private members of FAssetGateCancellationToken. */
	friend class FAssetGateCancellationTokenSource;

	/** Make the constructor explicit to prevent implicit conversions. 
	 * Useful here because a shared boolean flag and a cancellation token are conceptually different things,
	 * even if the token internally stores that flag.
	 */
	explicit FAssetGateCancellationToken(TSharedRef<FThreadSafeBool, ESPMode::ThreadSafe> InCancelledFlag);

	/** Non-nullable pointer to the shared cancellation flag. */
	TSharedRef<FThreadSafeBool, ESPMode::ThreadSafe> CancelledFlag;
};


/**
 * Owns a validation run's cancellation flag and is the only way to request cancellation.
 * 
 * The code that starts/orchestrates a validation run (a Cancel button, a CI timeout
 * watchdog, a user aborting a batch operation) holds the source. Everything else that
 * needs to observe cancellation should be given a FAssetGateCancellationToken via
 * GetToken(), not the source itself.
 * 
 * Conceptually:
 *	FAssetGateCancellationTokenSource
 *		        |
 *		        | owns/shared access
 *		        v
 *     shared FThreadSafeBool
 *		        ^
 *		        | observes
 *    FAssetGateCancellationToken
 *
 * - FAssetGateCancellationTokenSource is allowed to construct or manipulate token internals.
 * - FAssetCancellationToken stores a shared, thread-safe cancellation flag.
 * - The token can be safely passed around to long-running or asynchronous work.
 * - That work can periodically ask: "Has cancellation been requested?"
 * - The token source can request cancellation by setting the shared flag.
 */
class ASSETGATECORE_API FAssetGateCancellationTokenSource
{
public:
	FAssetGateCancellationTokenSource();

	/** Request cancellation. Safe to call from any thread. */
	void RequestCancel();

	/** @return true once RequestCancel() has been called. */
	bool IsCancellationRequested() const;

	/** @return a read-only token bound to this source's cancellation flag. */
	FAssetGateCancellationToken GetToken() const;

private:
	TSharedRef<FThreadSafeBool, ESPMode::ThreadSafe> CancelledFlag;
};
