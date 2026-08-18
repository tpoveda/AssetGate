using UnrealBuildTool;

public class AssetGateSourceControl : ModuleRules
{
	public AssetGateSourceControl(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"SourceControl",
			"AssetGateCore"
		});
	}
}