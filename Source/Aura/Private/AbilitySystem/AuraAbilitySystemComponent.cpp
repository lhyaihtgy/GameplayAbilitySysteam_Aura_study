// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

/**
 * @brief 当AbilityActorInfo被设置完成后调用的回调函数
 * @note AbilityActorInfo包含了能力系统组件所属的Actor、Controller等关键信息，
 *       只有当该信息初始化完成后，才能安全绑定能力相关的委托
 */
void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// 绑定"GameplayEffect应用到自身"的委托：当有GameplayEffect（比如buff/debuff/技能效果）应用到当前ASC时，
	// 自动触发EffectApplied回调函数
	// AddUObject：基于UObject的委托绑定方式，保证GC安全（对象销毁时委托自动解绑）
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::ClientEffectApplied);
	
}

//这个函数的作用是将角色的初始技能（通常在角色创建时或游戏开始时）添加到技能系统组件中，并立即激活一次这些技能
void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StarupAbilities)
{
	for(TSubclassOf<UGameplayAbility> AbilityClass:StarupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartUpInputTag);
			//下面这行代码的作用是将GameplayAbilitySpec添加到当前ASC中，使其生效，并不会激活一次技能
			GiveAbility(AbilitySpec);
			//GiveAbilityAndActivateOnce(AbilitySpec);
		}
		//将技能组中的技能增加到技能系统组件中标准步骤如下：
		//1.检查AbilityClass是否有效，防止空指针错误
		//check(AbilityClass);
		//2.创建一个GameplayAbilitySpec对象，包含技能类、等级、输入绑定等信息
		//FGameplayAbilitySpec AbilitySpec(AbilityClass,1);
		//3.将GameplayAbilitySpec添加到当前ASC中，使其生效，并不会激活一次技能
		//GiveAbility(AbilitySpec);
		//这个函数的作用是增加到技能系统组件中后立即激活一次技能，触发技能的开始阶段（比如播放动画、生成特效等），
		//GiveAbilityAndActivateOnce(AbilitySpec);//只是为了验证技能的可行性
	}
}

//这个函数的作用是当玩家按住某个输入标签时，尝试激活所有具有该输入标签的技能
void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;//输入的标签无效时直接返回，避免后续逻辑错误
	
	if (GetActivatableAbilities().Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No activatable abilities found!"));
	}
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		//下面这一行代码的作用是标记该 AbilitySpec 对应的输入为 “按下状态”；
		//触发该能力的 “输入按下” 回调（如 UGameplayAbility::InputPressed 虚函数，有我们自己定义逻辑效果）；
		//如果该能力配置为 “输入按下时激活”（比如普通攻击、跳跃能力），引擎会尝试激活这个 Gameplay Ability。
		//简单说明就是告诉技能你的按键被按下了，技能会根据自己的配置来决定是否要激活自己（比如有些技能需要持续按住才会激活，有些技能只需要按一下就会激活），
		AbilitySpecInputPressed(AbilitySpec);
		//下面代码的作用是检查当前技能是否具有玩家按下的输入标签，如果有，并且技能当前未激活，那么就尝试激活这个技能
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;//输入的标签无效时直接返回，避免后续逻辑错误
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			//简单说明就是告诉技能你的按键被松开了，技能会根据自己的配置来决定是否要停止自己（比如有些技能需要持续按住才会保持激活状态，一旦松开就会停止），
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

/**
 * @brief GameplayEffect应用到自身时的回调函数
 * @param AbilitySystemComponent 应用GameplayEffect的目标AbilitySystemComponent（此处即当前ASC自身）
 * @param EffectSpec 应用的GameplayEffect规格（包含效果的等级、上下文、标签、修改器等核心信息）
 * @param ActiveEffectHandle 已激活的GameplayEffect句柄（可通过该句柄查询/修改活跃的效果）
 * @note 该函数会在任何GameplayEffect（如加血、减伤、中毒buff）应用到当前ASC时被触发
 */
void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	
	//对widget Controller进行广播，广播的时候会给对应的响应函数传入一个Assert Tag
	EffectAssertTags.Broadcast(TagContainer);
	
}
