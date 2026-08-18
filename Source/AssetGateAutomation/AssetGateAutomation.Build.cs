using UnrealBuildTool;

public class AssetGateAutomation : ModuleRules
{
	public AssetGateAutomation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"AssetGateCore"
			}
		);
	}
}