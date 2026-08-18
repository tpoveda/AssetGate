// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "Validation/AssetGateDiagnosticTransport.h""

#if WITH_EDITOR

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAssetGate2DiagnosticTransportTest,
	"AssetGate.Core.Validation.DiagnosticTransport",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FAssetGate2DiagnosticTransportTest::RunTest(const FString& Parameters)
{
	FAssetGateDiagnosticTransport& Transport = FAssetGateDiagnosticTransport::Get();
	Transport.Clear();

	FAssetGateDiagnostic Diagnostic;
	Diagnostic.RuleId = TEXT("AssetGate2.Test.DiagnosticTransport");
	Diagnostic.AssetPath = FSoftObjectPath(TEXT("/Game/Test/Asset.Asset"));
	Diagnostic.PropertyPath = TEXT("DisplayName");
	Diagnostic.Id = TEXT("AssetGate2.Test.DiagnosticTransport:/Game/Test/Asset.Asset:DisplayName");
	Diagnostic.Message = FText::FromString(TEXT("Diagnostic transport should retain this result."));
	Diagnostic.Severity = EAssetGateDiagnosticSeverity::Warning;

	FAssetGateValidationRunContext Context = FAssetGateValidationRunContext::MakeManualFullProjectRun();
	Context.SetRuntimeMetadata(3, 0.75);
	Transport.EmitBatch({ Diagnostic }, &Context);

	const TArray<FAssetGateDiagnostic>& RecentDiagnostics = Transport.GetRecentDiagnostics();
	TestEqual(TEXT("Transport should store emitted diagnostics in memory"), RecentDiagnostics.Num(), 1);
	TestEqual(TEXT("Transport should preserve diagnostic id"), RecentDiagnostics[0].Id, Diagnostic.Id);
	TestEqual(TEXT("Transport should preserve asset path"), RecentDiagnostics[0].AssetPath.ToString(),
	          Diagnostic.AssetPath.ToString());
	TestEqual(TEXT("Transport should preserve severity"), RecentDiagnostics[0].Severity, Diagnostic.Severity);

	const TArray<FAssetGateDiagnosticPayload>& RecentPayloads = Transport.GetRecentPayloads();
	TestEqual(TEXT("Transport should retain structured payloads for UI/export use"), RecentPayloads.Num(), 1);
	TestEqual(
		TEXT("Structured payload should carry scope metadata for the editor UI"),
		RecentPayloads[0].RunMetadata.ScopeLabel, TEXT("Full project"));
	TestEqual(
		TEXT("Structured payload should carry asset-count metadata for the editor UI"),
		RecentPayloads[0].RunMetadata.AssetCount, 3);
	TestEqual(
		TEXT("Structured payload should carry elapsed-time metadata for the editor UI"),
		RecentPayloads[0].RunMetadata.ElapsedSeconds, 0.75);
	TestEqual(
		TEXT("Transport should keep a one-to-one mapping from diagnostic id to diagnostic payload"),
		RecentPayloads[0].DiagnosticIndex.Num(), 1);
	TestNotNull(TEXT("Diagnostic lookup should be available by stable ID"), Transport.FindDiagnostic(Diagnostic.Id));
	TestTrue(
		TEXT("Diagnostic lookup should return the original diagnostic value"),
		Transport.FindDiagnostic(Diagnostic.Id) != nullptr && Transport.FindDiagnostic(Diagnostic.Id)->Id == Diagnostic.
		Id);

	return true;
}

#endif
