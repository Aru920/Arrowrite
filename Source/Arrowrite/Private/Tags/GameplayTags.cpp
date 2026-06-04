// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tags/GameplayTags.h"

namespace ArrowriteGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Aim, "Input.Aim", "Input tag for aiming the bow.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Fire, "Input.Fire", "Input tag for drawing or firing the bow.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Bow_Aim, "Ability.Bow.Aim", "Ability tag for bow aiming.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Bow_Fire, "Ability.Bow.Fire", "Ability tag for bow draw and fire.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Aiming, "State.Aiming", "State tag applied while the player is aiming.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_DrawingBow, "State.DrawingBow", "State tag applied while the bow is being drawn.");
}
