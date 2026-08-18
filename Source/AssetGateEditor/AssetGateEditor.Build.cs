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
			"UnrealEd",
			"ContentBrowser",
			"AssetRegistry",
			"DataValidation",
			"AssetGateCore",
			"AssetGateRules",
			"AssetGateSourceControl"
		});
	}
}