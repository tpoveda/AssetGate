// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "Validation/AssetGateDiagnosticTransport.h"

DEFINE_LOG_CATEGORY_STATIC(LogAssetGateDiagnosticTransport, Log, All);

FAssetGateDiagnosticTransport::FAssetGateDiagnosticTransport()
{
}

FAssetGateDiagnosticTransport& FAssetGateDiagnosticTransport::Get()
{
	static FAssetGateDiagnosticTransport Instance;
	return Instance;
}

void FAssetGateDiagnosticTransport::Emit(const FAssetGateDiagnostic& Diagnostic)
{
	RecentDiagnostics.Add(Diagnostic);
	while (RecentDiagnostics.Num() > MaxStoredDiagnostics)
	{
		RecentDiagnostics.RemoveAt(0);
	}
	EmitToMessageLog(Diagnostic);
}

void FAssetGateDiagnosticTransport::EmitBatch(const TArray<FAssetGateDiagnostic>& Diagnostics)
{
	for (const FAssetGateDiagnostic& Diagnostic : Diagnostics)
	{
		Emit(Diagnostic);
	}
}

const TArray<FAssetGateDiagnostic>& FAssetGateDiagnosticTransport::GetRecentDiagnostics() const
{
	return RecentDiagnostics;
}

void FAssetGateDiagnosticTransport::Clear()
{
	RecentDiagnostics.Reset();
}

void FAssetGateDiagnosticTransport::SetMaxStoredDiagnostics(int32 InMaxStoredDiagnostics)
{
	MaxStoredDiagnostics = InMaxStoredDiagnostics;
	while (RecentDiagnostics.Num() > MaxStoredDiagnostics)
	{
		RecentDiagnostics.RemoveAt(0);
	}
}

EMessageSeverity::Type FAssetGateDiagnosticTransport::ToMessageSeverity(EAssetGateDiagnosticSeverity InSeverity)
{
	switch (InSeverity)
	{
	case EAssetGateDiagnosticSeverity::Info:
		return EMessageSeverity::Info;
	case EAssetGateDiagnosticSeverity::Warning:
		return EMessageSeverity::Warning;
	case EAssetGateDiagnosticSeverity::Blocker:
		return EMessageSeverity::Error;
	case EAssetGateDiagnosticSeverity::Error:
	default:
		return EMessageSeverity::Error;
	}
}

void FAssetGateDiagnosticTransport::EmitToMessageLog(const FAssetGateDiagnostic& Diagnostic)
{
	const FString RuleToken = Diagnostic.RuleId.IsNone() ? TEXT("UnknownRule") : Diagnostic.RuleId.ToString();
	const FString AssetToken = Diagnostic.AssetPath.IsNull() ? TEXT("UnknownAsset") : Diagnostic.AssetPath.ToString();
	const FString PropertyToken = Diagnostic.PropertyPath.IsEmpty() ? TEXT("N/A") : Diagnostic.PropertyPath;
	const FString MessageText = Diagnostic.Message.IsEmpty()
		? TEXT("AssetGate validation failed.")
		: Diagnostic.Message.ToString();

	const FString FormattedMessage = FString::Printf(
		TEXT("[AssetGate][%s] Asset=%s Property=%s %s"),
		*RuleToken,
		*AssetToken,
		*PropertyToken,
		*MessageText);

	UE_LOG(LogAssetGateDiagnosticTransport, Log, TEXT("%s"), *FormattedMessage);

	FMessageLog AssetGate2Log("AssetGate2");
	switch (Diagnostic.Severity)
	{
	case EAssetGateDiagnosticSeverity::Info:
		AssetGate2Log.Info()->AddToken(FTextToken::Create(Diagnostic.Message.IsEmpty()
			? FText::FromString(MessageText)
			: Diagnostic.Message));
		break;
	case EAssetGateDiagnosticSeverity::Warning:
		AssetGate2Log.Warning()->AddToken(FTextToken::Create(Diagnostic.Message.IsEmpty()
			? FText::FromString(MessageText)
			: Diagnostic.Message));
		break;
	case EAssetGateDiagnosticSeverity::Blocker:
	case EAssetGateDiagnosticSeverity::Error:
	default:
		AssetGate2Log.Error()->AddToken(FTextToken::Create(Diagnostic.Message.IsEmpty()
			? FText::FromString(MessageText)
			: Diagnostic.Message));
		break;
	}
}
