// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Validation/AssetGateValidatorBase.h"
#include "AssetGateValidator_AssetNamingPresence.generated.h"

/**
 * Flags assets that still carry the engine's auto-generated default name (for example,
 * "NewMaterial" or "NewDataAsset"), which usually indicates the asset was never renamed
 * after creation. 
 * 
 * As a non-abstract UEditorValidatorBase derivation, this class is discovered and 
 * registered automatically by UEditorValidatorSubsystem; no explicit registration 
 * call is required.
 */
UCLASS()
class ASSETGATECORE_API UAssetGateValidator_AssetNamingPresence : public UAssetGateValidatorBase
{
	GENERATED_BODY()

public:
	/** Stable rule identifier for this validator. */
	static const FName RuleId;

	//~ Begin UAssetGateValidatorBase interface
	virtual FName GetRuleId() const override;
	//~ End UAssetGateValidatorBase interface

protected:
	//~ Begin UAssetGateValidatorBase interface
	virtual FAssetGateValidationResult EvaluateAsset(const UObject* InAsset) const override;
	//~ End UAssetGateValidatorBase interface

private:
	/**
	 * Determine whether the specified asset name is still using the engine's auto-generated
	 * default naming convention (for example, "NewMaterial" or "NewDataAsset"). This function
	 * evaluates the asset name to identify if it has been renamed after its creation.
	 *
	 * @param AssetName The name of the asset to evaluate.
	 * @param ClassName The class name of the asset, used to assess the default naming pattern.
	 * @return True if the asset name reflects an auto-generated default name; otherwise, false.
	 */
	static bool IsUnrenamedDefaultAssetName(const FString& AssetName, const FString& ClassName);

};
