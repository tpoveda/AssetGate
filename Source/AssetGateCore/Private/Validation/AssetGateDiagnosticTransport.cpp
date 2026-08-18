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

void FAssetGateDiagnosticTransport::Emit(
	const FAssetGateDiagnostic& Diagnostic,
	const FAssetGateValidationRunContext* InRunContext)
{
	const FAssetGateDiagnostic Normalized = NormalizeDiagnostic(Diagnostic);
	RecentDiagnostics.Add(Diagnostic);
	while (RecentDiagnostics.Num() > MaxStoredDiagnostics)
	{
		RecentDiagnostics.RemoveAt(0);
	}

	if (!DiagnosticIndex.Contains(Normalized.Id))
	{
		DiagnosticIndex.Add(Normalized.Id, Normalized);
	}

	if (InRunContext)
	{
		RecordPayload({ Normalized }, InRunContext);
	}


	EmitToMessageLog(Diagnostic);
}

void FAssetGateDiagnosticTransport::EmitBatch(
	const TArray<FAssetGateDiagnostic>& Diagnostics,
	const FAssetGateValidationRunContext* InRunContext)
{
	TArray<FAssetGateDiagnostic> NormalizedDiagnostics;
	NormalizedDiagnostics.Reserve(Diagnostics.Num());

	for (const FAssetGateDiagnostic& Diagnostic : Diagnostics)
	{
		const FAssetGateDiagnostic Normalized = NormalizeDiagnostic(Diagnostic);
		NormalizedDiagnostics.Add(Normalized);
		RecentDiagnostics.Add(Normalized);
		if (!DiagnosticIndex.Contains(Normalized.Id))
		{
			DiagnosticIndex.Add(Normalized.Id, Normalized);
		}
		while (RecentDiagnostics.Num() > MaxStoredDiagnostics)
		{
			RecentDiagnostics.RemoveAt(0);
		}
		EmitToMessageLog(Normalized);
	}

	if (InRunContext != nullptr)
	{
		RecordPayload(NormalizedDiagnostics, InRunContext);
	}
}

const TArray<FAssetGateDiagnostic>& FAssetGateDiagnosticTransport::GetRecentDiagnostics() const
{
	return RecentDiagnostics;
}

const TArray<FAssetGateDiagnosticPayload>& FAssetGateDiagnosticTransport::GetRecentPayloads() const
{
	return RecentPayloads;
}

const FAssetGateDiagnostic* FAssetGateDiagnosticTransport::FindDiagnostic(const FString& DiagnosticId) const
{
	const FAssetGateDiagnostic* FoundDiagnostic = DiagnosticIndex.Find(DiagnosticId);
	return FoundDiagnostic;
}

const TMap<FString, FAssetGateDiagnostic>& FAssetGateDiagnosticTransport::GetDiagnosticIndex() const
{
	return DiagnosticIndex;
}

void FAssetGateDiagnosticTransport::Clear()
{
	RecentDiagnostics.Reset();
	RecentPayloads.Reset();
	DiagnosticIndex.Reset();
}

void FAssetGateDiagnosticTransport::SetMaxStoredDiagnostics(const int32 InMaxStoredDiagnostics)
{
	MaxStoredDiagnostics = FMath::Max(InMaxStoredDiagnostics, 1);
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

FAssetGateDiagnostic FAssetGateDiagnosticTransport::NormalizeDiagnostic(const FAssetGateDiagnostic& Diagnostic)
{
	FAssetGateDiagnostic Normalized = Diagnostic;
	if (!Normalized.Id.IsEmpty())
	{
		return Normalized;
	}

	Normalized.Id = FAssetGateValidationId::MakeDiagnosticId(
		Normalized.RuleId, Normalized.AssetPath,
		Normalized.PropertyPath);

	return Normalized;
}

void FAssetGateDiagnosticTransport::RecordPayload(
	const TArray<FAssetGateDiagnostic>& Diagnostics,
	const FAssetGateValidationRunContext* InRunContext)
{
	if (InRunContext == nullptr || Diagnostics.Num() == 0)
	{
		return;
	}

	FAssetGateDiagnosticPayload Payload;
	Payload.BatchId = FString::Printf(TEXT("Batch_%d"), RecentPayloads.Num() + 1);
	Payload.RunMetadata = InRunContext->Metadata;
	Payload.Diagnostics = Diagnostics;

	for (const FAssetGateDiagnostic& Diagnostic : Diagnostics)
	{
		if (!Diagnostic.Id.IsEmpty())
		{
			Payload.DiagnosticIndex.Add(Diagnostic.Id, Diagnostic);
		}
	}

	RecentPayloads.Add(Payload);
	while (RecentPayloads.Num() > MaxStoredPayloads)
	{
		RecentPayloads.RemoveAt(0);
	}
}
