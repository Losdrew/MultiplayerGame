// Copyright Artem Volovyi. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MultiplayerGameTarget : TargetRules
{
	public MultiplayerGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "MultiplayerGame" } );
	}
}
