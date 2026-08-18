// Copyright (c) 2026 Tomi Poveda. All Rights Reserved.


#include "SAssetGateValidationConsole.h"

#include "Editor.h"
#include "Validation/AssetGateDiagnosticTransport.h"
#include "Validation/AssetGateValidationTypes.h"

#include "SlateOptMacros.h"
#include "Misc/Paths.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"
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

	/**
	 * Return the editor-facing title for the first built-in naming validator.
	 *
	 * @param RuleId The rule identifier associated with the diagnostic.
	 * @return A human-readable rule title for the explanation panel.
	 */
	FText GetRuleTitle(const FName RuleId)
	{
		if (RuleId == TEXT("AssetGate2.Naming.Presence"))
		{
			return LOCTEXT("NamingPresenceRuleTitle", "Rename default-generated assets");
		}

		return RuleId.IsNone()
			? LOCTEXT("UnknownRuleTitle", "Unknown AssetGate2 rule")
			: FText::FromName(RuleId);
	}

	/**
	 * Returns a short rationale for the first built-in naming validator.
	 *
	 * @param RuleId The rule identifier associated with the diagnostic.
	 * @return Plain-language explanation shown to the user.
	 */
	FText GetRuleSummary(const FName RuleId)
	{
		if (RuleId == TEXT("AssetGate2.Naming.Presence"))
		{
			return LOCTEXT(
				"NamingPresenceRuleSummary",
				"Default engine-generated names hide asset intent and make large content libraries harder to search, review, and maintain. Rename the asset to something purpose-specific so the package is understandable in the Content Browser, validation output, and source control history.");
		}

		return LOCTEXT(
			"UnknownRuleSummary",
			"AssetGate2 recorded this issue, but this rule does not yet expose a custom rationale summary.");
	}
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetGateValidationConsole::Construct(const FArguments& InArgs)
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
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("AssetQualityConsoleTitle", "Asset Quality Console"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f, 0.0f, 0.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("AssetQualityConsoleSubtitle",
					              "Review recorded validation issues, inspect why they failed, and jump directly to the affected asset."))
					.AutoWrapText(true)
					.ColorAndOpacity(FLinearColor(0.72f, 0.72f, 0.72f))
				]
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
							"AssetGate validation dispatch is ready for the next validation slice.")));
					return FReply::Handled();
				})
			]
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(8.0f, 0.0f, 8.0f, 8.0f)
		[
			SNew(SSplitter)
			+ SSplitter::Slot()
			.Value(0.67f)
			[
				SAssignNew(ListView, SListView<TSharedPtr<FAssetGateConsoleRow>>)
				.ListItemsSource(&Rows)
				.SelectionMode(ESelectionMode::Single)
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
				.OnSelectionChanged_Lambda(
					[this](const TSharedPtr<FAssetGateConsoleRow>& InItem, ESelectInfo::Type SelectInfo)
					{
						SelectedRow = InItem;
					})
				.HeaderRow
				(
					SNew(SHeaderRow)
					+ SHeaderRow::Column(TEXT("Asset")).DefaultLabel(LOCTEXT("AssetHeader", "Asset"))
					+ SHeaderRow::Column(TEXT("Rule")).DefaultLabel(LOCTEXT("RuleHeader", "Rule"))
					+ SHeaderRow::Column(TEXT("Severity")).DefaultLabel(LOCTEXT("SeverityHeader", "Severity"))
					+ SHeaderRow::Column(TEXT("Reason")).DefaultLabel(LOCTEXT("ReasonHeader", "Reason"))
					+ SHeaderRow::Column(TEXT("Action")).DefaultLabel(LOCTEXT("ActionHeader", "Action"))
					)
			]
			+ SSplitter::Slot()
			.Value(0.33f)
			[
				SNew(SBorder)
				.Padding(12.0f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("ExplanationPanelTitle", "Why is this failing?"))
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 10.0f, 0.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT(
							"ExplanationPanelEmptyState",
							"Select a validation result to inspect the rule explanation."))
						.AutoWrapText(true)
						.Visibility_Lambda([this]()
						{
							return SelectedRow.IsValid() ? EVisibility::Visible : EVisibility::Collapsed;
						})
						.ColorAndOpacity(FLinearColor(0.72f, 0.72f, 0.72f))
					]
					+ SVerticalBox::Slot()
					.FillHeight(1.0f)
					.Padding(0.0f, 10.0f, 0.0f, 0.0f)
					[
						SNew(SScrollBox)
						.Visibility_Lambda([this]()
						{
							return SelectedRow.IsValid() ? EVisibility::Collapsed : EVisibility::Visible;
						})
						+ SScrollBox::Slot()
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
							.AutoHeight()
							[
								SNew(STextBlock)
								.Text(LOCTEXT("ExplanationRuleLabel", "Rule"))
								.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
							]
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(0.0f, 4.0f, 0.0f, 0.0f)
							[
								SNew(STextBlock)
								.Text_Lambda([this]()
								{
									if (!SelectedRow.IsValid())
									{
										return FText::GetEmpty();
									}

									const FAssetGateDiagnostic* Diagnostic = FAssetGateDiagnosticTransport::Get().
										FindDiagnostic(SelectedRow->DiagnosticId);
									return Diagnostic != nullptr
										? GetRuleTitle(Diagnostic->RuleId)
										: FText::FromString(SelectedRow->RuleName);
								})
								.AutoWrapText(true)
							]
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(0.0f, 12.0f, 0.0f, 0.0f)
							[
								SNew(STextBlock)
								.Text(LOCTEXT("ExplanationAssetLabel", "Asset"))
								.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
							]
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(0.0f, 4.0f, 0.0f, 0.0f)
							[
								SNew(STextBlock)
								.Text_Lambda([this]()
								{
									if (!SelectedRow.IsValid())
									{
										return FText::GetEmpty();
									}

									return SelectedRow->AssetPath.IsNull()
										? FText::FromString(SelectedRow->AssetName)
										: FText::FromString(SelectedRow->AssetPath.ToString());
								})
								.AutoWrapText(true)
							]
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(0.0f, 12.0f, 0.0f, 0.0f)
							[
								SNew(STextBlock)
								.Text(LOCTEXT("ExplanationSummaryLabel", "Summary"))
								.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
							]
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(0.0f, 4.0f, 0.0f, 0.0f)
							[
								SNew(STextBlock)
								.Text_Lambda([this]()
								{
									if (!SelectedRow.IsValid())
									{
										return FText::GetEmpty();
									}

									const FAssetGateDiagnostic* Diagnostic = FAssetGateDiagnosticTransport::Get().
										FindDiagnostic(SelectedRow->DiagnosticId);
									if (Diagnostic == nullptr)
									{
										return FText::FromString(SelectedRow->Message);
									}

									const FText RuleSummary = GetRuleSummary(Diagnostic->RuleId);
									const FText DiagnosticMessage = Diagnostic->Message.IsEmpty()
										? FText::FromString(SelectedRow->Message)
										: Diagnostic->Message;

									return FText::Format(
										LOCTEXT("ExplanationSummaryFormat", "{0}\n\nCurrent result: {1}"),
										RuleSummary,
										DiagnosticMessage);
								})
								.AutoWrapText(true)
							]
						]
					]
				]
			]
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetGateValidationConsole::RefreshRows()
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
