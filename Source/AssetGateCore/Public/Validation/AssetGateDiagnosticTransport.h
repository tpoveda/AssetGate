// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#pragma once

#include "AssetGateValidationRunContext.h"
#include "AssetGateValidationTypes.h"

#include "CoreMinimal.h"
#include "Logging/MessageLog.h"

/**
 * Structured payload recorded for one validation result batch.
 * 
 * This object is intentionally UI/reporting friendly: it contains the run metadata and the
 * exact diagnostics emitted for a given submission so the same data can be rendered in the
 * editor, the browser, and the command line without re-running validation.
 */
struct ASSETGATECORE_API FAssetGateDiagnosticPayload
{
	/** Synthetic batch identifier for a submission. */
	FString BatchId;

	/** Metadata describing how the run was triggered and what scope it covered. */
	FAssetGateValidationRunMetadata RunMetadata;

	/** Diagnostics emitted for this batch. */
	TArray<FAssetGateDiagnostic> Diagnostics;

	/** One-to-one lookup of diagnostic ID to the emitted diagnostic. */
	TMap<FString, FAssetGateDiagnostic> DiagnosticIndex;
};

/**
 * Provides diagnostic transport functionality for the AssetGate validation system.
 *
 * This class acts as a centralized mechanism for emitting, storing, and accessing
 * diagnostics related to asset validation. Diagnostics can either be emitted individually
 * or in batches and are forwarded to the Unreal Engine message log while also maintaining
 * an in-memory history for further operations.
 */
class ASSETGATECORE_API FAssetGateDiagnosticTransport
{
public:
	/** Return the singleton transport instance used by the validation pipeline. */
	static FAssetGateDiagnosticTransport& Get();

	/**
	 * Emit a single diagnostic to the central transport for processing.
	 *
	 * This method is responsible for forwarding the provided diagnostic to the editor message log,
	 * ensuring it appears in the appropriate Unreal Engine logging systems. It also stores the
	 * diagnostic in memory so it can be accessed via history-related operations like recent diagnostics
	 * retrieval.
	 *
	 * @param Diagnostic The diagnostic object to be emitted, containing details such as the associated
	 *        rule ID, asset path, severity, and a human-readable message.
	* @param InRunContext An optional pointer to the validation run context providing additional
	 *                     metadata about the validation execution. Defaults to nullptr if not provided.
	 */
	void Emit(const FAssetGateDiagnostic& Diagnostic, const FAssetGateValidationRunContext* InRunContext = nullptr);

	/**
	 * Emit a batch of diagnostic results and supports contextual metadata for the validation process.
	 *
	 * This method is used to transmit a collection of diagnostic results from a validation run,
	 * optionally including metadata to provide context about the validation execution.
	 *
	 * @param Diagnostics The array of diagnostic results to be emitted as part of the batch.
	 * @param InRunContext An optional pointer to the validation run context providing additional
	 *                     metadata about the validation execution. Defaults to nullptr if not provided.
	 */
	void EmitBatch(
		const TArray<FAssetGateDiagnostic>& Diagnostics,
		const FAssetGateValidationRunContext* InRunContext = nullptr);

	/**
	 * Retrieves a reference to the array containing the most recent diagnostics.
	 *
	 * This method provides access to an in-memory history of diagnostics emitted
	 * through the transport. The diagnostics stored in this array are limited
	 * by the configured maximum storage capacity to ensure efficient memory usage.
	 *
	 * @return A constant reference to the array of recent FAssetGateDiagnostic objects
	 *         retained by the transport for history-related operations.
	 */
	const TArray<FAssetGateDiagnostic>& GetRecentDiagnostics() const;

	/**
	 * Retrieves the most recent batch of diagnostic payloads.
	 *
	 * This method provides access to the collection of payloads produced by the latest validation results,
	 * allowing consumers to review diagnostics and associated metadata.
	 *
	 * @return A constant reference to an array of diagnostic payloads representing recent validation results.
	 */
	const TArray<FAssetGateDiagnosticPayload>& GetRecentPayloads() const;

	/**
	 * Finds and retrieves a diagnostic record by its unique identifier.
	 *
	 * This method searches through the available diagnostics and returns a pointer to the
	 * diagnostic that matches the given identifier, if found.
	 *
	 * @param DiagnosticId The unique identifier of the diagnostic to search for.
	 * @return A pointer to the corresponding FAssetGateDiagnostic if found, otherwise nullptr.
	 */
	const FAssetGateDiagnostic* FindDiagnostic(const FString& DiagnosticId) const;

	/**
	 * Retrieves a mapping of diagnostic keys to their corresponding diagnostic details.
	 *
	 * This method provides access to the indexed diagnostics, enabling efficient
	 * lookup and analysis of diagnostics data based on their unique string keys.
	 *
	 * @return A constant reference to a map where each key is a string identifier,
	 *         and each value is an associated diagnostic object.
	 */
	const TMap<FString, FAssetGateDiagnostic>& GetDiagnosticIndex() const;

	/** Clear the recent diagnostics buffer for a new validation session. */
	void Clear();

	/**
	 * Configure the maximum number of diagnostics that can be stored in the system. Ensures
	 * older diagnostics are discarded when the specified limit is reached to maintain storage efficiency.
	 *
	 * @param InMaxStoredDiagnostics The maximum number of diagnostics to be stored.
	 */
	void SetMaxStoredDiagnostics(int32 InMaxStoredDiagnostics);

private:
	FAssetGateDiagnosticTransport();

	/**
	 * Convert an EAssetGateDiagnosticSeverity value to the corresponding EMessageSeverity::Type value.
	 *
	 * This utility function maps the severity levels defined in the AssetGate diagnostic system
	 * to the Unreal Engine message severity system, enabling consistent interpretation of
	 * diagnostic levels across the framework.
	 *
	 * @param InSeverity The diagnostic severity from the AssetGate system to be converted.
	 * @return The corresponding EMessageSeverity::Type that represents the same severity level.
	 */
	static EMessageSeverity::Type ToMessageSeverity(EAssetGateDiagnosticSeverity InSeverity);

	/**
	 * Emits a single diagnostic to the Unreal Engine message log.
	 *
	 * @param Diagnostic The diagnostic information to be logged, containing details
	 *        such as the rule ID, asset path, severity, and a human-readable message.
	 */
	static void EmitToMessageLog(const FAssetGateDiagnostic& Diagnostic);

	/**
	 * Normalize the provided diagnostic information to ensure consistent formatting
	 * and data integrity.
	 *
	 * @param Diagnostic The diagnostic data to normalize.
	 * @return A normalized diagnostic object with consistent formatting and validated structure.
	 */
	static FAssetGateDiagnostic NormalizeDiagnostic(const FAssetGateDiagnostic& Diagnostic);

	/**
	 * Record a structured payload containing diagnostics and associated metadata for a validation run.
	 *
	 * This method captures the diagnostic results and relevant contextual information
	 * for further analysis, processing, or reporting.
	 *
	 * @param Diagnostics An array of diagnostic results emitted during the validation process.
	 * @param InRunContext A pointer to the context associated with the current validation run.
	 */
	void RecordPayload(
		const TArray<FAssetGateDiagnostic>& Diagnostics,
		const FAssetGateValidationRunContext* InRunContext);

	/**
	 * Stores a list of the most recent diagnostics generated by the AssetGate validation pipeline.
	 *
	 * This array is managed by the diagnostic transport system and holds an in-memory history of
	 * diagnostics for access during operations like troubleshooting or analyzing recent validation
	 * results. The storage capacity of this array is limited by the configured maximum, ensuring
	 * efficient memory usage by discarding the oldest diagnostics when the capacity is exceeded.
	 */
	TArray<FAssetGateDiagnostic> RecentDiagnostics;

	/**
	 * Collection of structured diagnostic payloads recorded from recent validation result batches.
	 *
	 * Contains metadata and diagnostics from submitted validations, enabling consistent rendering
	 * across multiple platforms such as editor, browser, and command line without requiring
	 * re-validation.
	 */
	TArray<FAssetGateDiagnosticPayload> RecentPayloads;

	/**
	 * A mapping of string keys to diagnostic data for asset validation.
	 *
	 * This structure serves as an indexed collection where each key corresponds to a unique asset,
	 * and its associated value contains detailed diagnostic information. It is designed to facilitate
	 * efficient lookups and storage of validation results, enabling diagnostics to be accessed
	 * and used for reporting or further processing.
	 */
	TMap<FString, FAssetGateDiagnostic> DiagnosticIndex;

	/**
	 * Define the maximum number of diagnostics that can be stored in memory by the diagnostic transport system.
	 *
	 * This value dictates the capacity of the internal storage buffer that retains recent diagnostics
	 * emitted through the system. Once the limit is reached, older diagnostics are discarded to
	 * maintain efficient memory usage. Adjusting this parameter can influence the amount of diagnostic
	 * history retained for operations such as troubleshooting or analysis.
	 */
	int32 MaxStoredDiagnostics{ 1024 };

	/**
	 * Maximum number of payloads that can be stored for diagnostic processing.
	 *
	 * This value defines the limit on how many diagnostic payloads can be retained
	 * simultaneously, allowing efficient management of memory usage while ensuring
	 * adequate diagnostic storage capacity.
	 */
	int32 MaxStoredPayloads{ 128 };

};
