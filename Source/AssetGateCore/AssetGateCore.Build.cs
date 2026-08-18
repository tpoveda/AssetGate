using UnrealBuildTool;

public class AssetGateCore : ModuleRules
{
	public AssetGateCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"DataValidation"
			}
		);
	}
}