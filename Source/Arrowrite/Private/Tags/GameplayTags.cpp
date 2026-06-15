// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tags/GameplayTags.h"

namespace ArrowriteGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Aim, "Input.Aim", "Input tag for aiming the bow.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Fire, "Input.Fire", "Input tag for drawing or firing the bow.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Bow_Aim, "Ability.Bow.Aim", "Ability tag for bow aiming.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Bow_Fire, "Ability.Bow.Fire", "Ability tag for bow draw and fire.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Player_Death, "Ability.Player.Death", "Ability tag for player death.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Player_HitReact, "Ability.Player.HitReact", "Ability tag for player hit reaction.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Aiming, "State.Aiming", "State tag applied while the player is aiming.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_DrawingBow, "State.DrawingBow", "State tag applied while the bow is being drawn.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Bow_Firing, "State.Bow.Firing", "State tag applied while the bow fire sequence is active.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Bow_Reloading, "State.Bow.Reloading", "State tag applied while the bow is reloading and cannot fire.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Dead, "State.Dead", "State tag applied when a player is dead.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_HitReacting, "State.HitReacting", "State tag applied while a player is reacting to a hit.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Animation_EnableAiming, "Event.Animation.EnableAiming", "Gameplay event sent by animation when the bow reaches the aiming pose.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Animation_DisableAiming, "Event.Animation.DisableAiming", "Gameplay event sent by animation when the bow leaves the aiming pose.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Bow_SpawnArrow, "Event.Bow.SpawnArrow", "Gameplay event sent when the fired arrow should spawn and launch.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Bow_ReloadArrow, "Event.Bow.ReloadArrow", "Gameplay event sent when the next preview arrow should be shown on the bow.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Player_HitReact, "Event.Player.HitReact", "Gameplay event sent when a player should play hit reaction.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Shared_SetByCaller_BaseDamage, "Shared.SetByCaller.BaseDamage", "Set-by-caller magnitude for base damage.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Bow, "Weapon.Bow", "Weapon tag for bow equipment.");
}
