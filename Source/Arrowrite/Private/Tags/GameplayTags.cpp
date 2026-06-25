// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tags/GameplayTags.h"

namespace ArrowriteGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Input_Aim, "Input.Aim");
	UE_DEFINE_GAMEPLAY_TAG(Input_Fire, "Input.Fire");
	UE_DEFINE_GAMEPLAY_TAG(Input_ArrowMenu, "Input.ArrowMenu");
	UE_DEFINE_GAMEPLAY_TAG(Input_Dash, "Input.Dash");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Bow_Aim, "Ability.Bow.Aim");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Bow_Fire, "Ability.Bow.Fire");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Bow_ArrowMenu, "Ability.Bow.ArrowMenu");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Death, "Ability.Player.Death");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_HitReact, "Ability.Player.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Dash, "Ability.Player.Dash");

	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Player_Dash, "Cooldown.Player.Dash");

	UE_DEFINE_GAMEPLAY_TAG(State_Aiming, "State.Aiming");
	UE_DEFINE_GAMEPLAY_TAG(State_DrawingBow, "State.DrawingBow");
	UE_DEFINE_GAMEPLAY_TAG(State_Bow_Firing, "State.Bow.Firing");
	UE_DEFINE_GAMEPLAY_TAG(State_Bow_Reloading, "State.Bow.Reloading");
	UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");
	UE_DEFINE_GAMEPLAY_TAG(State_HitReacting, "State.HitReacting");
	UE_DEFINE_GAMEPLAY_TAG(State_Dashing, "State.Dashing");
	UE_DEFINE_GAMEPLAY_TAG(State_Burning, "State.Burning");
	UE_DEFINE_GAMEPLAY_TAG(State_Poisoned, "State.Poisoned");
	UE_DEFINE_GAMEPLAY_TAG(State_Frozen, "State.Frozen");
	UE_DEFINE_GAMEPLAY_TAG(State_Stamina_RegenBlocked, "State.Stamina.RegenBlocked");

	UE_DEFINE_GAMEPLAY_TAG(Event_Animation_EnableAiming, "Event.Animation.EnableAiming");
	UE_DEFINE_GAMEPLAY_TAG(Event_Animation_DisableAiming, "Event.Animation.DisableAiming");
	UE_DEFINE_GAMEPLAY_TAG(Event_Bow_SpawnArrow, "Event.Bow.SpawnArrow");
	UE_DEFINE_GAMEPLAY_TAG(Event_Bow_ReloadArrow, "Event.Bow.ReloadArrow");
	UE_DEFINE_GAMEPLAY_TAG(Event_Player_HitReact, "Event.Player.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Event_Player_Death, "Event.Player.Death");

	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Status_Burning, "GameplayCue.Status.Burning");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Status_Poisoned, "GameplayCue.Status.Poisoned");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Status_Frozen, "GameplayCue.Status.Frozen");

	UE_DEFINE_GAMEPLAY_TAG(Shared_SetByCaller_BaseDamage, "Shared.SetByCaller.BaseDamage");
	UE_DEFINE_GAMEPLAY_TAG(Shared_SetByCaller_StaminaCost, "Shared.SetByCaller.StaminaCost");

	UE_DEFINE_GAMEPLAY_TAG(Weapon_Bow, "Weapon.Bow");

	UE_DEFINE_GAMEPLAY_TAG(Arrow_Normal, "Arrow.Normal");
	UE_DEFINE_GAMEPLAY_TAG(Arrow_Fire, "Arrow.Fire");
	UE_DEFINE_GAMEPLAY_TAG(Arrow_Poison, "Arrow.Poison");
	UE_DEFINE_GAMEPLAY_TAG(Arrow_Freeze, "Arrow.Freeze");
}
