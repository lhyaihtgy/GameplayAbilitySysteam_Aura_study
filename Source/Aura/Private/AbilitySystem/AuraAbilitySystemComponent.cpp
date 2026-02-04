// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

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
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::EffectApplied);
	
	const FAuraGameplayTags& GameplayTags =  FAuraGameplayTags::Get();
	//测试这个标签是否真的创建
	GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Red,FString::Printf(TEXT("Tag: %s"),*GameplayTags.Attributes_Secondary_Armor.ToString()));
}
/**
 * @brief GameplayEffect应用到自身时的回调函数
 * @param AbilitySystemComponent 应用GameplayEffect的目标AbilitySystemComponent（此处即当前ASC自身）
 * @param EffectSpec 应用的GameplayEffect规格（包含效果的等级、上下文、标签、修改器等核心信息）
 * @param ActiveEffectHandle 已激活的GameplayEffect句柄（可通过该句柄查询/修改活跃的效果）
 * @note 该函数会在任何GameplayEffect（如加血、减伤、中毒buff）应用到当前ASC时被触发
 */
void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	
	//对widget Controller进行广播，广播的时候会给对应的响应函数传入一个Assert Tag
	EffectAssertTags.Broadcast(TagContainer);
	
}
