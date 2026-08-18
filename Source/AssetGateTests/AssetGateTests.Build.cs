using UnrealBuildTool;

public class AssetGateTests : ModuleRules
{
	public AssetGateTests(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"UnrealEd",
			"AssetRegistry",
			"DataValidation",
			"AssetGateCore"
		});
	}
}