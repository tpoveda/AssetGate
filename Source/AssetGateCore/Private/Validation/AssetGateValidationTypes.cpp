// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.


#include "Validation/AssetGateValidationTypes.h"

void FAssetGateValidationResult::AddDiagnostic(const FAssetGateDiagnostic& Diagnostic)
{
	Diagnostics.Add(Diagnostic);
}

bool FAssetGateValidationResult::IsEmpty() const
{
	return Diagnostics.IsEmpty();
}

FAssetGateValidationResult FAssetGateValidationResult::MakeValid()
{
	FAssetGateValidationResult Result;
	Result.Outcome = EAssetGateValidationOutcome::Valid;
	return Result;
}

FAssetGateValidationResult FAssetGateValidationResult::MakeInvalid(TArray<FAssetGateDiagnostic>&& InDiagnostics)
{
	FAssetGateValidationResult Result;
	Result.Outcome = EAssetGateValidationOutcome::Invalid;
	Result.Diagnostics = MoveTemp(InDiagnostics);
	return Result;
}

FAssetGateValidationResult FAssetGateValidationResult::MakeNotValidated(TArray<FAssetGateDiagnostic>&& InDiagnostics)
{
	FAssetGateValidationResult Result;
	Result.Outcome = EAssetGateValidationOutcome::NotValidated;
	Result.Diagnostics = MoveTemp(InDiagnostics);
	return Result;
}

FString FAssetGateValidationId::MakeDiagnosticId(
	const FName RuleId,
	const FSoftObjectPath& AssetPath,
	const FString& PropertyPath)
{
	const FString RuleToken = RuleId.IsNone() ? TEXT("UnknownRule") : RuleId.ToString();
	const FString AssetToken = AssetPath.IsNull() ? TEXT("UnknownAsset") : AssetPath.ToString();

	if (PropertyPath.IsEmpty())
	{
		return FString::Printf(TEXT("%s:%s"), *RuleToken, *AssetToken);
	}

	return FString::Printf(TEXT("%s:%s:%s"), *RuleToken, *AssetToken, *PropertyPath);
}
