using UnrealBuildTool;

public class AssetGateRules : ModuleRules
{
	public AssetGateRules(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"AssetGateCore"
		});
	}
}