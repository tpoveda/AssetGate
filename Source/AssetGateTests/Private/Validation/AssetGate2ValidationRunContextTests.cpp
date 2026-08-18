// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "Validation/AssetGateDiagnosticTransport.h""

#if WITH_EDITOR

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAssetGate2DiagnosticTransportTest,
	"AssetGate2.Core.Validation.DiagnosticTransport",
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

	Transport.Emit(Diagnostic);

	const TArray<FAssetGateDiagnostic>& RecentDiagnostics = Transport.GetRecentDiagnostics();
	TestEqual(TEXT("Transport should store emitted diagnostics in memory"), RecentDiagnostics.Num(), 1);
	TestEqual(TEXT("Transport should preserve diagnostic id"), RecentDiagnostics[0].Id, Diagnostic.Id);
	TestEqual(TEXT("Transport should preserve asset path"), RecentDiagnostics[0].AssetPath.ToString(),
	          Diagnostic.AssetPath.ToString());
	TestEqual(TEXT("Transport should preserve severity"), RecentDiagnostics[0].Severity, Diagnostic.Severity);

	return true;
}

#endif
