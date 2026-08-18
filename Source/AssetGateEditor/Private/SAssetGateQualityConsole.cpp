// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.


#include "SAssetGateQualityConsole.h"

#include "Editor.h"
#include "Validation/AssetGateDiagnosticTransport.h"
#include "Validation/AssetGateValidationTypes.h"

#include "SlateOptMacros.h"
#include "Misc/Paths.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Views/SListView.h"

#define LOCTEXT_NAMESPACE "SAssetQualityConsole"

namespace
{
	/**
	 * Converts a diagnostic severity value into the human-readable label used in the console.
	 *
	 * @param Diagnostic The diagnostic whose severity is being rendered.
	 * @return A display-friendly text label for the severity.
	 */
	FText GetSeverityTextForDiagnostic(const FAssetGateDiagnostic& Diagnostic)
	{
		switch (Diagnostic.Severity)
		{
		case EAssetGateDiagnosticSeverity::Info:
			return FText::FromString(TEXT("Info"));
		case EAssetGateDiagnosticSeverity::Warning:
			return FText::FromString(TEXT("Warning"));
		case EAssetGateDiagnosticSeverity::Blocker:
			return FText::FromString(TEXT("Blocker"));
		case EAssetGateDiagnosticSeverity::Error:
		default:
			return FText::FromString(TEXT("Error"));
		}
	}
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetGateQualityConsole::Construct(const FArguments& InArgs)
{
	RefreshRows();

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("AssetQualityConsoleTitle", "Asset Quality Console"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSpacer)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("AssetGateRefresh", "Refresh"))
				.OnClicked_Lambda([this]()
				{
					RefreshRows();
					return FReply::Handled();
				})
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("AssetGateValidationSelected", "Validate Selected"))
				.OnClicked_Lambda([this]()
				{
					FMessageLog AssetGateLog("AssetGate");
					AssetGateLog.Info()->AddToken(
						FTextToken::Create(LOCTEXT(
							"ValidateSelectedPlaceholder",
							"AssetGate2 validation dispatch is ready for the next validation slice.")));
					return FReply::Handled();
				})
			]
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SAssignNew(ListView, SListView<TSharedPtr<FAssetGateConsoleRow>>)
			.ListItemsSource(&Rows)
			.OnGenerateRow_Lambda([this](
				TSharedPtr<FAssetGateConsoleRow> InItem,
				const TSharedRef<STableViewBase>& OwnerTable)
				{
					return SNew(STableRow<TSharedPtr<FAssetGateConsoleRow>>, OwnerTable)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.FillWidth(1.5f)
							[
								SNew(STextBlock)
								.Text(FText::FromString(InItem->AssetName))
							]
							+ SHorizontalBox::Slot()
							.FillWidth(1.0f)
							[
								SNew(STextBlock)
								.Text(FText::FromString(InItem->RuleName))
							]
							+ SHorizontalBox::Slot()
							.FillWidth(0.5f)
							[
								SNew(STextBlock)
								.Text(FText::FromString(InItem->Severity))
							]
							+ SHorizontalBox::Slot()
							.FillWidth(2.5f)
							[
								SNew(STextBlock)
								.Text(FText::FromString(InItem->Message))
								.AutoWrapText(true)
							]
							+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								SNew(SButton)
								.Text(LOCTEXT("JumpToAssetButton", "Jump"))
								.OnClicked_Lambda([this, InItem]()
								{
									if (!InItem.IsValid() || InItem->AssetPath.IsNull())
									{
										return FReply::Handled();
									}

									UObject* Asset = InItem->AssetPath.TryLoad();
									if (!Asset)
									{
										return FReply::Handled();
									}

									GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Asset);

									return FReply::Handled();
								})
							]
						];
				})
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetGateQualityConsole::RefreshRows()
{
	Rows.Reset();

	for (const FAssetGateDiagnostic& Diagnostic : FAssetGateDiagnosticTransport::Get().GetRecentDiagnostics())
	{
		const TSharedPtr<FAssetGateConsoleRow> Row = MakeShared<FAssetGateConsoleRow>();
		Row->AssetName = Diagnostic.AssetPath.IsNull()
			? TEXT("UnknownAsset")
			: FPaths::GetCleanFilename(Diagnostic.AssetPath.ToString());
		Row->RuleName = Diagnostic.RuleId.IsNone() ? TEXT("UnknownRule") : Diagnostic.RuleId.ToString();
		Row->Severity = GetSeverityTextForDiagnostic(Diagnostic).ToString();
		Row->Message = Diagnostic.Message.IsEmpty() ? TEXT("No message provided.") : Diagnostic.Message.ToString();
		Row->AssetPath = Diagnostic.AssetPath;
		Rows.Add(Row);
	}

	if (ListView.IsValid())
	{
		ListView->RequestListRefresh();
	}
}

#undef LOCTEXT_NAMESPACE
