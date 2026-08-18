// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.

#include "Engine/DataAsset.h"
#include "Misc/AutomationTest.h"
#include "Misc/DataValidation.h"
#include "Validation/Validators/AssetGateValidator_AssetNamingPresence.h"

#if WITH_EDITOR

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAssetGateValidator_AssetNamingPresenceTest,
	"AssetGate.Core.Validation.AssetNamingPresence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FAssetGateValidator_AssetNamingPresenceTest::RunTest(const FString& Parameters)
{
	UAssetGateValidator_AssetNamingPresence* Validator = NewObject<UAssetGateValidator_AssetNamingPresence>();
	if (!TestNotNull(TEXT("Validator instance should be created"), Validator))
	{
		return false;
	}

	TestEqual(
		TEXT("Validator exposes its stable rule id"),
		Validator->GetRuleId(),
		UAssetGateValidator_AssetNamingPresence::RuleId);

	// An asset that still carries its engine-generated default name should fail validation.
	UDataAsset* UnrenamedAsset = NewObject<UDataAsset>(
		GetTransientPackage(),
		UDataAsset::StaticClass(),
		TEXT("NewDataAsset0"));
	if (TestNotNull(TEXT("Unrenamed test asset should be created"), UnrenamedAsset))
	{
		FDataValidationContext Context;
		const EDataValidationResult Result = Validator->ValidateLoadedAsset(
			FAssetData(UnrenamedAsset), UnrenamedAsset, Context);
		TestEqual(TEXT("Unrenamed asset should be reported invalid"), Result, EDataValidationResult::Invalid);
	}

	// An asset renamed away from the default should pass validation.
	UDataAsset* RenamedAsset = NewObject<UDataAsset>(
		GetTransientPackage(),
		UDataAsset::StaticClass(),
		TEXT("PlayerInventoryDefaults"));
	if (TestNotNull(TEXT("Renamed test asset should be created"), RenamedAsset))
	{
		FDataValidationContext Context;
		const EDataValidationResult Result = Validator->ValidateLoadedAsset(
			FAssetData(RenamedAsset), RenamedAsset, Context);
		TestEqual(TEXT("Renamed asset should be reported valid"), Result, EDataValidationResult::Valid);
	}

	return true;
}

#endif
