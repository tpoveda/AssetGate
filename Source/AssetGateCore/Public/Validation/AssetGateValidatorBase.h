// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetGateValidationTypes.h"
#include "EditorValidatorBase.h"
#include "UObject/Object.h"
#include "AssetGateValidatorBase.generated.h"

/**
 * Base class for AssetGate validators integrated with Unreal Data validation.
 * 
 * Derivations implement EvaluateAsset and emit AssetGate diagnostics.
 */
UCLASS(Abstract)
class ASSETGATECORE_API UAssetGateValidatorBase : public UEditorValidatorBase
{
	GENERATED_BODY()

public:
	/**
	 * @return The stable AssetGate rule identifier represented by this validator. 
	 */
	UFUNCTION(BlueprintCallable, Category = "AssetGate|Validation")
	virtual FName GetRuleId() const PURE_VIRTUAL(UAssetGateValidatorBase::GetRuleId, return NAME_None;);

protected:
	/**
	 * Evaluate the specified asset against the rules implemented by the derived validator class.
	 * This method is expected to be implemented in derived classes to perform validation logic specific to the asset.
	 *
	 * @param InAsset The asset to be evaluated. It provides context and data for the implementation of validation rules.
	 *                The parameter is a pointer to a UObject, and it must not be null. Derived classes should ensure
	 *                to handle validation for the appropriate asset types.
	 * @return A FAssetGateValidationResult containing the outcome of the evaluation. The result includes the validation
	 *         status (Valid, Invalid, or NotValidated) and any associated diagnostic messages providing additional context.
	 */
	virtual FAssetGateValidationResult EvaluateAsset(const UObject* InAsset) const PURE_VIRTUAL(
		UAssetGateValidatorBase::EvaluateAsset, return FAssetGateValidationResult::MakeNotValidated(););

	/**
	 * Determine whether the specified asset can be validated by the validator implementation.
	 * This function evaluates the asset data, the asset object itself, and the validation context to decide
	 * if validation logic should be applied to the asset.
	 *
	 * @param InAssetData The asset metadata to be evaluated. Provides information such as asset tags, name, and path.
	 * @param InObject The asset object to be validated. This provides the actual UObject that the validation may operate on.
	 * @param InContext The validation context, which tracks the state and results of the validation process.
	 *                  It allows validators to emit messages and control the validation flow.
	 * @return True if the validator can process the asset for validation, or false otherwise.
	 */
	virtual bool CanValidateAsset_Implementation(
		const FAssetData& InAssetData, UObject* InObject,
		FDataValidationContext& InContext) const override;

	/**
	 * Implement validation logic for a loaded asset by evaluating it according to the rules defined
	 * by the derived class. This method is invoked as part of the Unreal Engine asset validation system.
	 *
	 * @param InAssetData Metadata associated with the asset being validated. Provides information such as
	 *                    asset name, path, and tags, which can be used to support validation logic.
	 * @param InAsset The actual UObject representing the loaded asset. This parameter provides access to
	 *                the asset's data and properties necessary for performing validation.
	 * @param Context The validation context, which tracks and manages the state of validation. It supports
	 *                reporting errors, warnings, or information regarding the validation results.
	 * @return A result of type EDataValidationResult, indicating the outcome of the validation. Possible values include
	 *         Valid (validation passed), Invalid (validation failed), or NotValidated (validation not performed).
	 */
	virtual EDataValidationResult ValidateLoadedAsset_Implementation(
		const FAssetData& InAssetData, UObject* InAsset,
		FDataValidationContext& Context) override;

	/**
	 * Append a diagnostic message to the validation errors in the provided data validation context.
	 * This method is used to report validation issues identified during asset evaluation, associating them
	 * with the current validation process.
	 *
	 * @param Diagnostic The diagnostic information to be appended. Includes details about the rule identifier,
	 *                   asset context, severity, and human-readable messages describing the issue.
	 * @param InContext The validation context where the diagnostic should be registered. It manages the state of
	 *                  validation and tracks errors, warnings, and informational messages for the asset being evaluated.
	 */
	void AppendDiagnosticToValidationErrors(
		const FAssetGateDiagnostic& Diagnostic,
		FDataValidationContext& InContext) const;
};
