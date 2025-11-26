// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

UAuraAttributeSet::UAuraAttributeSet()
{
    //这个函数就是通过ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);宏生成的
    InitHealth(100.0f);
    InitMana(100.0f);
    InitMaxMana(100.0f);
    InitMaxHealth(100.0f);
}

/**
 * @brief 重写GAS属性集的生命周期复制属性注册函数
 * 核心作用：向UE网络系统注册需要跨服务器-客户端同步的属性，定义属性的同步规则（条件、是否强制触发回调）
 * 所有标记为Replicated的GAS属性，必须在此函数中通过DOREPLIFETIME系列宏注册，否则无法同步
 * @param OutLifetimeProps 输出参数，存储当前属性集所有需要同步的属性信息（由UE内部处理后续同步逻辑）
 */
void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    // 调用父类实现，确保父类中声明的可同步属性也能被注册（如GAS内置基础属性，避免遗漏）
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 注册Health属性的同步规则：
    // 1. DOREPLIFETIME_CONDITION_NOTIFY：带同步条件和回调触发策略的注册宏
    // 2. 模板参数：当前属性集类（UAuraAttributeSet）、要同步的属性（Health）
    // 3. COND_None：同步条件为"无限制"（只要属性变更，服务器就会同步到客户端，不额外过滤）
    // 4. REPNOTIFY_Always：强制触发OnRep_Health回调（无论同步的新值与客户端旧值是否一致，均执行回调）
    // 用途：确保客户端能实时响应生命值变更（如UI刷新、受伤特效），即使值未变也需触发（如特殊逻辑判定）
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);

    // 注册MaxHealth属性的同步规则（与Health逻辑一致）：
    // 同步条件无限制，强制触发OnRep_MaxHealth回调
    // 用途：最大生命值变更时（如升级、装备加成），客户端需及时刷新UI上限、属性面板等
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth,COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana,COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana,COND_None, REPNOTIFY_Always);
}

/**
 * @brief 生命值（Health）网络同步回调函数
 * 触发时机：服务器同步Health属性到客户端后，由UE自动调用（因注册时指定REPNOTIFY_Always，无条件触发）
 * @param OldHealth 同步前客户端本地的Health旧值（用于对比变更幅度，如计算掉血量）
 * 核心逻辑：通过GAS内置宏转发回调，触发属性变更的全局通知（供其他系统监听，如UI、技能逻辑）
 */
void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
    // GAMEPLAYATTRIBUTE_REPNOTIFY：GAS提供的属性同步通知宏，核心作用：
    // 1. 验证属性的有效性（确保是GAS注册的合法GameplayAttribute）
    // 2. 触发属性变更的全局事件（OnGameplayAttributeChanged），让其他监听该属性的系统（如UI组件、技能系统）响应
    // 3. 自动处理属性的"旧值→新值"切换逻辑，无需手动维护
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

/**
 * @brief 最大生命值（MaxHealth）网络同步回调函数
 * 触发时机：服务器同步MaxHealth属性到客户端后，由UE自动调用（无条件触发）
 * @param OldMaxHealth 同步前客户端本地的MaxHealth旧值（用于对比变更幅度，如升级增加的最大血量）
 * 核心逻辑：与OnRep_Health一致，通过GAS宏转发通知，确保全局系统感知最大生命值变更
 */
void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
    // 调用GAS内置宏，触发MaxHealth属性的全局变更通知
    // 后续可由UI系统监听该事件，刷新最大血量显示；或由技能系统调整基于最大血量的效果（如百分比回血技能）
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

//于上面的一致
void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}
