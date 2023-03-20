// Copyright Artem Volovyi. All Rights Reserved.

using UnrealBuildTool;

public class MultiplayerGame : ModuleRules
{
	public MultiplayerGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.AddRange(new string[]
        {
            "MultiplayerGame", 
            "MultiplayerGame/AbilitySystem",
            "MultiplayerGame/AbilitySystem/Abilities",
            "MultiplayerGame/AbilitySystem/Attributes",
            "MultiplayerGame/AbilitySystem/Executions",
            "MultiplayerGame/Character",
            "MultiplayerGame/Equipment",
            "MultiplayerGame/GameModes",
            "MultiplayerGame/Input",
            "MultiplayerGame/Physics",
            "MultiplayerGame/Player",
            "MultiplayerGame/System",
            "MultiplayerGame/UI",
            "MultiplayerGame/Weapons" 
        });
	
        PublicDependencyModuleNames.AddRange(new string[] 
        {
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HeadMountedDisplay",
            "NetCore"
        });

		PrivateDependencyModuleNames.AddRange(new string[]
        {
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
            "EnhancedInput"
        });

		PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate",
            "SlateCore"
        });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
