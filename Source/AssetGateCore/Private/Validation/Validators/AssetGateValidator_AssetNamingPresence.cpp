// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "Validation/Validators/AssetGateValidator_AssetNamingPresence.h"

const FName UAssetGateValidator_AssetNamingPresence::RuleId(TEXT("AssetGate.Naming.Presence"));

FName UAssetGateValidator_AssetNamingPresence::GetRuleId() const
{
	return RuleId;
}

FAssetGateValidationResult UAssetGateValidator_AssetNamingPresence::EvaluateAsset(const UObject* InAsset) const
{
	if (!IsValid(InAsset))
	{
		return FAssetGateValidationResult::MakeNotValidated();
	}

	const FString AssetName = InAsset->GetName();
	const FString ClassName = InAsset->GetClass() ? InAsset->GetClass()->GetName() : FString();

	if (!IsUnrenamedDefaultAssetName(AssetName, ClassName))
	{
		return FAssetGateValidationResult::MakeValid();
	}

	FAssetGateDiagnostic Diagnostic;
	Diagnostic.RuleId = RuleId;
	Diagnostic.AssetPath = FSoftObjectPath(InAsset);
	Diagnostic.Id = FAssetGateValidationId::MakeDiagnosticId(RuleId, Diagnostic.AssetPath);
	Diagnostic.Severity = EAssetGateDiagnosticSeverity::Warning;
	Diagnostic.Message = FText::Format(
		NSLOCTEXT(
			"AssetGate", "NamingPresenceMessage",
			"Asset '{0}' still uses its engine-generated default name; rename it to reflect its purpose."),
		FText::FromString(AssetName));

	TArray<FAssetGateDiagnostic> Diagnostics;
	Diagnostics.Add(MoveTemp(Diagnostic));

	return FAssetGateValidationResult::MakeInvalid(MoveTemp(Diagnostics));
}

bool UAssetGateValidator_AssetNamingPresence::IsUnrenamedDefaultAssetName(
	const FString& AssetName,
	const FString& ClassName)
{
	if (ClassName.IsEmpty())
	{
		return false;
	}

	const FString DefaultNamePrefix = FString::Printf(TEXT("New%s"), *ClassName);
	return AssetName.StartsWith(DefaultNamePrefix, ESearchCase::IgnoreCase);
}
