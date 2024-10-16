// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MTVS_AirJet_Final : ModuleRules
{
	public MTVS_AirJet_Final(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core"
			, "CoreUObject"
			, "Engine"
			, "InputCore"
			, "EnhancedInput"
			, "Niagara"
			, "UMG"
			, "OnlineSubsystem"
			, "OnlineSubsystemSteam"
			, "OnlineSubsystemUtils"
			, "PixelStreaming"
			, "PixelStreamingPlayer"
			, "HTTP"
			, "Json"
			, "JsonUtilities"
			, "GameplayTasks"
			, "AudioCapture"
			, "AudioMixer"
			, "OpenCVHelper"
			, "OpenCV" 
			, "WebBrowser"
			, "Slate"
			, "SlateCore"
		});
	}
}
