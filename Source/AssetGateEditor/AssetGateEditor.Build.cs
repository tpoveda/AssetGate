using UnrealBuildTool;

public class AssetGateEditor : ModuleRules
{
	public AssetGateEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"InputCore",
			"UnrealEd",
			"LevelEditor",
			"ContentBrowser",
			"AssetRegistry",
			"AssetTools",
			"ToolMenus",
			"DataValidation",
			"AssetGateCore",
			"AssetGateRules",
			"AssetGateSourceControl"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"WorkspaceMenuStructure"
		});
	}
}