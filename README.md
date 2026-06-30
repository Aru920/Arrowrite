Arrowrite

Arrowrite is a multiplayer third-person bow combat game built in Unreal Engine 5.5. The project is designed as a portfolio piece focused on C++ gameplay systems, Blueprint-driven UI, Gameplay Ability System combat, replicated projectiles, and a complete deathmatch loop.

Overview

The core idea is simple: players host or join a match, choose arrow types, aim and fire a bow, apply damage/status effects, die, respawn, and fight until the round ends. Most gameplay rules are written in C++, while Blueprints are used for ability graphs, UI wiring, animation setup, and visual/audio polish.

This project is my main multiplayer portfolio game. It shows how I connect C++ systems with Blueprint-facing events so designers can build UI and effects without rewriting gameplay logic.

Engine And Plugins

- Unreal Engine 5.5
- C++ gameplay module: Arrowrite
- Gameplay Ability System
- Enhanced Input
- UMG UI
- Niagara/Cascade/VFX assets
- Listen-server multiplayer
- HDRI Backdrop enabled for scene lighting

Main Features

- Host and join flow with player name entry
- Listen-server multiplayer deathmatch
- Warmup countdown before the match starts
- Match timer, score tracking, kill feed, match end screen, and round reset
- Bow aiming and firing built with Gameplay Abilities
- Replicated arrow projectile travel, impact, sticking, and cleanup
- Multiple arrow data assets for normal, fire, poison, and freeze arrows
- Status effect gameplay effects for burning, poison, and frozen states
- Floating damage numbers above hit characters
- Hit marker feedback for confirmed hits
- Death, respawn countdown, and combat HUD visibility handling
- Health and stamina UI driven by attribute change events
- Sprint stamina drain and stamina regeneration delay
- Selected arrow HUD icon and arrow selection menu
- Control hint widget for player inputs
- Camera shake, impact sounds, arrow sounds, and UI feedback hooks

Gameplay Loop

1. Player opens the main menu.
2. Player hosts a listen server or joins an existing server by address.
3. Players enter names before hosting or joining.
4. The match waits until enough players are connected.
5. A warmup countdown starts.
6. Players fight with bow and arrow abilities.
7. Damage numbers, hit markers, health/stamina UI, and kill feed update during combat.
8. Dead players see a respawn countdown while combat HUD elements are hidden.
9. The match ends when the timer finishes.
10. Winner/final score UI appears, then the round resets.

Portfolio Breakdown

1. Bow Combat

This section shows the main combat input flow: aiming, crosshair visibility, bow pose state, arrow spawn, arrow launch, projectile travel, impact handling, damage, and impact FX.

Important assets and files:

- Content/Assets/Character/GameplayAbilities/PlayerAbilities/GA_BowAim.uasset
- Content/Assets/Character/GameplayAbilities/PlayerAbilities/GA_BowFire.uasset
- Content/Assets/Weapon/BP_BowWeapon.uasset
- Content/Assets/Weapon/BP_Arrowprojectile.uasset
- Source/Arrowrite/Public/AbilitySystem/Abilities/Player/PlayerGameplayAbility.h
- Source/Arrowrite/Private/AbilitySystem/Abilities/Player/PlayerGameplayAbility.cpp
- Source/Arrowrite/Public/Projectiles/ArrowProjectile.h
- Source/Arrowrite/Private/Projectiles/ArrowProjectile.cpp

What this demonstrates:

- Using Gameplay Abilities for combat actions
- Keeping reusable player ability helpers in C++
- Avoiding stale Blueprint references by fetching the current weapon live
- Applying selected arrow data to spawned projectiles
- Replicating projectile launch and impact behavior for server and clients

2. Arrow Selection

Arrow types are handled through data assets. The arrow menu lets the player select an arrow type, and the selected data controls damage, stamina cost, projectile mesh, trail FX, impact FX, sounds, and status effects.

Important assets and files:

- Content/Assets/Widgets/WBP_ArrowSelectionMenu.uasset
- Content/Assets/Widgets/WBP_ArrowSelectionOption.uasset
- Content/Assets/Character/GameplayAbilities/PlayerAbilities/GA_BowArrowMenu.uasset
- Content/Assets/Character/Widget/WBP_SelectedArrowIcon.uasset
- Content/Assets/Weapon/DataAssets/DA_Arrow_Normal.uasset
- Content/Assets/Weapon/DataAssets/DA_Arrow_Fire.uasset
- Content/Assets/Weapon/DataAssets/DA_Arrow_Poison.uasset
- Content/Assets/Weapon/DataAssets/DA_Arrow_Freeze.uasset
- Source/Arrowrite/Public/Projectiles/ArrowDataAsset.h
- Source/Arrowrite/Public/Player/PlayerEquipmentComponent.h
- Source/Arrowrite/Private/Player/PlayerEquipmentComponent.cpp

What this demonstrates:

- Data-driven gameplay tuning
- Replicated selected arrow state
- Blueprint UI reading C++ equipment state
- Reusing the same fire ability with different arrow data

3. Health And Stamina

Health and stamina are exposed through a combat UI component. UMG widgets bind to attribute changes and update bars, text, low-stamina feedback, and combat visibility.

Important assets and files:

- Content/Assets/Character/Widget/WBP_PlayerHUD.uasset
- Content/Assets/Character/Widget/WBP_HealthBar.uasset
- Content/Assets/Character/Widget/WBP_StaminaBar.uasset
- Content/Assets/Character/Widget/WBP_NotEnoughStamina.uasset
- Content/Assets/Character/Widget/WBP_ControlHints.uasset
- Source/Arrowrite/Public/UI/PlayerCombatUIComponent.h
- Source/Arrowrite/Private/UI/PlayerCombatUIComponent.cpp
- Source/Arrowrite/Public/AbilitySystem/Player/PlayerAttributeSet.h
- Source/Arrowrite/Private/AbilitySystem/Player/PlayerAttributeSet.cpp
- Source/Arrowrite/Public/Character/Player/PlayerCharacter.h
- Source/Arrowrite/Private/Character/Player/PlayerCharacter.cpp

What this demonstrates:

- Attribute-driven UI
- Health/stamina percentage helpers
- Smooth bar interpolation in widgets
- Sprint stamina drain handled by gameplay code
- Low stamina feedback handled through HUD widgets

4. Aiming Locomotion

The player uses normal locomotion when not aiming, then switches into bow aiming movement and animation state while aiming. The aim state is replicated so the correct pose is visible in multiplayer.

Important assets and files:

- Content/Assets/Character/ABP_Player.uasset
- Content/Assets/Character/GameplayAbilities/PlayerAbilities/GA_BowAim.uasset
- Content/Assets/Character/Animations/AimOffset
- Source/Arrowrite/Public/Character/Player/PlayerCharacter.h
- Source/Arrowrite/Private/Character/Player/PlayerCharacter.cpp
- Source/Arrowrite/Public/Animation/Player/PlayerAnimInstance.h
- Source/Arrowrite/Private/Animation/Player/PlayerAnimInstance.cpp
- Source/Arrowrite/Private/Tags/GameplayTags.cpp

What this demonstrates:

- Gameplay tag based aim state
- Replicated aim pose state
- Blueprint animation state driven by C++ character state
- Separating gameplay state from animation presentation

5. Hit React, Death, Respawn, And Match End

Combat feedback includes hit react direction, damage numbers, status visuals, death state, respawn countdown, kill feed, scoreboard, and match-end UI.

Important assets and files:

- Content/Assets/Character/GameplayAbilities/PlayerAbilities/GA_PlayerHitReaction.uasset
- Content/Assets/Character/GameplayAbilities/PlayerAbilities/GA_PlayerDeath.uasset
- Content/Assets/Character/Widget/WBP_DamageNumber.uasset
- Content/Assets/Character/Widget/BP_DamageNumbersA.uasset
- Content/Assets/Character/Widget/WBP_HitMarker.uasset
- Content/Assets/Character/Widget/WBP_RespawnCountdown.uasset
- Content/Assets/Character/Widget/WBP_KillFeed.uasset
- Content/Assets/Character/Widget/WBP_KillFeedEntry.uasset
- Content/Assets/Character/Widget/WBP_MatchEnd.uasset
- Content/Assets/Character/Widget/WBP_ScoreboardTop.uasset
- Source/Arrowrite/Public/Core/DeathmatchGameMode.h
- Source/Arrowrite/Private/Core/DeathmatchGameMode.cpp
- Source/Arrowrite/Public/Core/DeathmatchGameState.h
- Source/Arrowrite/Private/Core/DeathmatchGameState.cpp
- Source/Arrowrite/Public/Player/GamePlayerState.h
- Source/Arrowrite/Private/Player/GamePlayerState.cpp
- Source/Arrowrite/Public/Player/GamePlayerController.h
- Source/Arrowrite/Private/Player/GamePlayerController.cpp

What this demonstrates:

- Server-owned death and respawn flow
- Client UI events from C++ controller hooks
- Replicated scoreboard and kill feed data
- Match result replication
- Round reset and combat state cleanup

6. Host And Join

The project uses a simple listen-server flow. Players can host a game, join by address, set a display name, and travel into the match map.

Important assets and files:

- Content/Assets/Widgets/WBP_MainMenu.uasset
- Content/Assets/Widgets/WBP_HostMenu.uasset
- Content/Assets/Widgets/WBP_JoinMenu.uasset
- Content/Assets/Widgets/WBP_MatchStartStatus.uasset
- Content/Assets/Character/BP_MainMenuGameMode.uasset
- Content/Assets/Character/BP_MainMenuPlayerController.uasset
- Source/Arrowrite/Public/BlueprintFunctionLibraries/MenuTravelBlueprintLibrary.h
- Source/Arrowrite/Private/BlueprintFunctionLibraries/MenuTravelBlueprintLibrary.cpp
- Source/Arrowrite/Public/Core/ArrowriteGameInstance.h
- Source/Arrowrite/Private/Core/ArrowriteGameInstance.cpp
- Source/Arrowrite/Public/Core/DeathmatchGameMode.h
- Source/Arrowrite/Private/Core/DeathmatchGameMode.cpp

What this demonstrates:

- Blueprint-callable C++ menu travel helpers
- Host listen server travel
- Join by IP/address
- Player name sanitization and travel options
- Server-side player name setup in InitNewPlayer
- Warmup countdown when enough players join

Controls

Default controls are configured through Enhanced Input assets.

- Move: WASD
- Look: Mouse
- Aim: Right Mouse Button
- Fire: Left Mouse Button while aiming
- Arrow Menu: R
- Sprint: Shift
- Host/Join/Quit: Main menu buttons

Project Structure

Config/        Unreal project settings
Content/       Maps, Blueprints, widgets, animations, VFX, audio, and assets
Source/        C++ gameplay code

Generated folders such as Binaries, Intermediate, Saved, and DerivedDataCache should not be committed.

How To Run

1. Open the project with Unreal Engine 5.5.
2. Let Unreal rebuild project files if prompted.
3. Open the main menu map.
4. Press Play.
5. Use Host to create a listen server.
6. Use a second PIE client or standalone instance to join.

For local multiplayer testing:

Number of Players: 2
Net Mode: Play As Listen Server
Play Mode: New Editor Window

For join testing on the same machine:

Server Address: 127.0.0.1

Recommended Portfolio Media

- Main menu host/join flow
- Two local players connected in PIE
- Warmup countdown and FIGHT text
- Bow aiming and arrow firing from client and server
- Arrow travel, trail, impact, and sticking
- Normal, fire, poison, and freeze arrow examples
- Damage numbers above hit players
- Hit marker on confirmed hit
- Death montage and respawn countdown
- Kill feed and scoreboard updates
- Match end screen and round reset

Notes On Assets

This project uses a mix of custom gameplay code, Blueprint systems, Unreal assets, and third-party art/audio/VFX packs. The gameplay systems, networking logic, ability flow, UI wiring, and C++ architecture are the main portfolio focus. Third-party content is used as presentation support and should be credited or handled according to its license before public distribution.

Current Status

Arrowrite is in portfolio polish stage. The main gameplay loop is playable, with remaining work mostly focused on UI polish, arena presentation, final Blueprint cleanup, screenshots, gameplay recording, and a final multiplayer test pass.
