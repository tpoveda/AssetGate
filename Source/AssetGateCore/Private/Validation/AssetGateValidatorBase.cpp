// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.


#include "Validation/AssetGateValidatorBase.h"

#include "Logging/TokenizedMessage.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "AssetGate"

bool UAssetGateValidatorBase::CanValidateAsset_Implementation(
	const FAssetData& InAssetData,
	UObject* InObject,
	FDataValidationContext& InContext) const
{
	return IsValid(InObject) || InAssetData.IsValid();
}

EDataValidationResult UAssetGateValidatorBase::ValidateLoadedAsset_Implementation(
	const FAssetData& InAssetData,
	UObject* InAsset,
	FDataValidationContext& Context)
{
	if (!IsValid(InAsset))
	{
		Context.AddMessage(
			EMessageSeverity::Type::Error,
			LOCTEXT(
				"InvalidAsset",
				"AssetGate validator received an invalid asset reference."));
		return EDataValidationResult::NotValidated;
	}

	const FAssetGateValidationResult Result = EvaluateAsset(InAsset);

	for (const FAssetGateDiagnostic& Diagnostic : Result.Diagnostics)
	{
		AppendDiagnosticToValidationErrors(Diagnostic, Context);
	}

	switch (Result.Outcome)
	{
	case EAssetGateValidationOutcome::Valid:
		return EDataValidationResult::Valid;
	case EAssetGateValidationOutcome::Invalid:
		return EDataValidationResult::Invalid;
	default:
		return EDataValidationResult::NotValidated;
	}
}

void UAssetGateValidatorBase::AppendDiagnosticToValidationErrors(
	const FAssetGateDiagnostic& Diagnostic,
	FDataValidationContext& InContext) const
{
	const FString RuleToken = Diagnostic.RuleId.IsNone() ? TEXT("UnknownRule") : Diagnostic.RuleId.ToString();
	const FString DiagnosticId = Diagnostic.Id.IsEmpty()
		? FAssetGateValidationId::MakeDiagnosticId(Diagnostic.RuleId, Diagnostic.AssetPath, Diagnostic.PropertyPath)
		: Diagnostic.Id;
	const FString AssetToken = Diagnostic.AssetPath.IsNull() ? TEXT("UnknownAsset") : Diagnostic.AssetPath.ToString();
	const FString PropertyToken = Diagnostic.PropertyPath.IsEmpty() ? TEXT("N/A") : Diagnostic.PropertyPath;

	const FText Message = Diagnostic.Message.IsEmpty()
		? LOCTEXT("EmptyDiagnosticMessage", "AssetGate validation failed.")
		: Diagnostic.Message;

	const FText FormattedMessage = FText::Format(
		LOCTEXT("ValidationMessageFormat", "[AssetGate][{0}] ({1}) Asset={2} Property={3} {4}"),
		FText::FromString(RuleToken),
		FText::FromString(DiagnosticId),
		FText::FromString(AssetToken),
		FText::FromString(PropertyToken),
		Message);

	const EMessageSeverity::Type Severity = (Diagnostic.Severity == EAssetGateDiagnosticSeverity::Info)
		? EMessageSeverity::Info
		: (Diagnostic.Severity == EAssetGateDiagnosticSeverity::Warning
			? EMessageSeverity::Warning
			: EMessageSeverity::Error);

	InContext.AddMessage(Severity, FormattedMessage);
}

#undef LOCTEXT_NAMESPACE
