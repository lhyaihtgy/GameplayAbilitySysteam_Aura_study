# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an Unreal Engine 5.3 project implementing a Gameplay Ability System (GAS) for an action RPG called "Aura". The project demonstrates multiplayer-ready ability systems with attribute management, input handling, and projectile-based spells.

## Build Commands

This is an Unreal Engine C++ project. Build through:
- **Visual Studio**: Open `Aura.sln` and build the solution
- **Unreal Editor**: Right-click `Aura.uproject` → "Generate Visual Studio project files", then open in editor
- **Command Line**: Use Unreal Build Tool (UBT) via `Engine/Build/BatchFiles/Build.bat`

No traditional test runner is configured. Testing is done through Unreal Editor's Play-in-Editor (PIE) and packaged builds.

## Architecture

### Core GAS Implementation

**Ability System Component Ownership**:
- **Player Characters**: ASC lives on `AAuraPlayerState` (survives respawns, proper for multiplayer)
- **AI/Enemies**: ASC lives directly on `AAuraEnemyCharacter` (simpler, no respawn needed)
- Both implement `IAbilitySystemInterface::GetAbilitySystemComponent()`

**Attribute System** (`UAuraAttributeSet`):
- **Primary Attributes**: Strength, Intelligence, Resilience, Vigor (base stats)
- **Secondary Attributes**: Armor, ArmorPenetration, BlockChance, CriticalHit*, HealthRegen, ManaRegen, MaxHealth, MaxMana (derived from primary)
- **Vital Attributes**: Health, Mana (current values)
- Uses `ATTRIBUTE_ACCESSORS` macro for Get/Set/Init methods
- Network replication via `ReplicatedUsing` with `OnRep_*` callbacks
- `PreAttributeChange()` clamps values before modification
- `PostGameplayEffectExecute()` handles post-modification logic

**Initialization Flow**:
1. `AAuraCharacterBase::InitAbilitySystemInfo()` - Called when ASC is ready
2. `InitializeDefaultAttributes()` - Applies GE for Primary → Secondary → Vital attributes
3. `AddCharacterAbilities()` - Grants startup abilities (server-only via `HasAuthority()`)

### Gameplay Tags

Centralized in `FAuraGameplayTags` singleton:
- `Attributes.Primary.*` - Primary attribute tags
- `Attributes.Secondary.*` - Secondary attribute tags
- `InputTag.*` - Input action tags (LMB, RMB, 1-4)
- Initialize via `InitializeNativeGameplayTags()` called early in startup

### Input System

Uses Enhanced Input with GAS integration:
- `UAuraInputConfig` (DataAsset) maps `UInputAction` → `FGameplayTag`
- `UAuraInputComponent` extends `UEnhancedInputComponent` with tag-based binding
- `UAuraAbilitySystemComponent::AbilityInputTagHeld/Released()` activates abilities by tag
- Input flows: Hardware → Enhanced Input → InputTag → ASC → Ability Activation

### Ability Classes

**Base**: `UAuraGameplayAbility` (extends `UGameplayAbility`)
**Projectile Spells**: `UAuraProjectileSpell`
- `SpawnProjectile()` - Blueprint-callable, spawns `AAuraProjectile` at target location
- Uses `ActivateAbility()` override for custom activation logic

**Ability Tasks**: `UTargetDataUnderMouse`
- Async task for getting mouse cursor world location
- Broadcasts `ValidData` delegate with target location
- **Known Issue**: Fixed multiplayer sync bug where client-only data wasn't replicated to server

### Multiplayer Considerations

- **Ability Activation**: Must use `ServerTryActivateAbility()` for client-initiated abilities
- **Target Data**: Use `UAbilityTask_WaitTargetData` or custom tasks with proper replication
- **Effect Application**: `ClientEffectApplied()` RPC ensures UI updates on clients
- **Level Replication**: `AAuraPlayerState::Level` uses `ReplicatedUsing=OnRep_Level`

### UI Architecture

- `AAuraHUD` - Main HUD class
- `UAuraUserWidget` - Base widget with `SetWidgetController()` pattern
- **Widget Controllers**: `UAuraWidgetController`, `UOverlayWidgetController`, `UAttributeMenuWidgetController`
  - Controllers act as ViewModels, bridging ASC/AttributeSet data to UI
  - Bind to ASC delegates for reactive updates

### Key Patterns

**Gameplay Effect Application**:
```cpp
ApplyEffectToself(TSubclassOf<UGameplayEffect> GameplayEffect, float level)
```
- Used for attribute initialization and buff/debuff application

**Combat Interface** (`ICombatInterface`):
- `GetCombatSocketLocation()` - Returns weapon tip location for projectile spawn
- Implemented by all combat-capable characters

**Asset Management**:
- `UAuraAssetManager` - Custom asset manager for preloading game data
- Configure in `DefaultEngine.ini` with `AssetManagerClassName`

## Common Workflows

**Adding New Ability**:
1. Create C++ class inheriting from `UAuraGameplayAbility` or `UAuraProjectileSpell`
2. Create Blueprint child in `Content/Blueprints/AbilitySystem/Abilities/`
3. Add to character's `StartupAbilities` array in Blueprint
4. Map input tag in `InputConfig` DataAsset if input-triggered

**Adding New Attribute**:
1. Add `FGameplayAttributeData` property to `UAuraAttributeSet.h` with `ATTRIBUTE_ACCESSORS` macro
2. Add `UPROPERTY` with `ReplicatedUsing=OnRep_AttributeName`
3. Implement `OnRep_AttributeName()` callback
4. Register in `GetLifetimeReplicatedProps()`
5. Add tag to `FAuraGameplayTags` and initialize in `InitializeNativeGameplayTags()`
6. Create Gameplay Effect Blueprint to modify the attribute

**Debugging Multiplayer**:
- Use `HasAuthority()` checks to verify server vs client execution
- Enable GAS logging: `showdebug abilitysystem` in console
- Check replication with `net.PackageMap` console commands
- Test with PIE using 2+ players (Editor Preferences → Play → Multiplayer Options)

## Project Structure

```
Source/Aura/
├── Public/
│   ├── AbilitySystem/
│   │   ├── Abilities/          # Gameplay abilities
│   │   ├── AbilityTasks/       # Async ability tasks
│   │   ├── Data/               # DataAssets for attributes
│   │   ├── ModMagCalc/         # Magnitude calculators (MMC)
│   │   ├── AuraAbilitySystemComponent.h
│   │   └── AuraAttributeSet.h
│   ├── Character/              # Character classes (base, player, enemy)
│   ├── Player/                 # PlayerController, PlayerState
│   ├── Input/                  # Enhanced Input integration
│   ├── UI/                     # Widget and WidgetController classes
│   ├── Actor/                  # Projectiles, effect actors
│   ├── Interaction/            # Interface classes
│   └── AuraGameplayTags.h      # Centralized gameplay tags
└── Private/                    # Implementation files mirror Public/
```

## Dependencies

- **GameplayAbilities Plugin**: Core GAS functionality (enabled in .uproject)
- **Enhanced Input**: Modern input system for UE5
- **UMG**: UI widget system
- **Replication Graph**: (Implicit) for multiplayer support

## Notes

- All Chinese comments in code are original developer notes - preserve them
- Recent commit fixed mouse target data sync bug in multiplayer (see `TargetDataUnderMouse`)
- Animation notifies trigger abilities via `GameplayEvent` with tags
- Weapon socket name configured per-character in Blueprint (`WeaponTipSocketName`)
