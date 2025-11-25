// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

AAuraPlayerState::AAuraPlayerState()
{
	// 1. 创建项目自定义的能力系统组件（UAuraAbilitySystemComponent），命名为"AbilitySystem"
	// 核心作用：作为GAS（Gameplay Ability System）的核心载体，负责管理角色的技能（Abilities）、游戏玩法效果（如buff/debuff）、属性计算等核心逻辑
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySysteam");

	// 2. 启用能力系统组件的网络复制功能
	// 关键意义：让服务器端的能力相关状态（如技能触发结果、效果生效/移除、属性变更）能跨网络同步到客户端，是多人游戏中GAS正常工作的必需配置
	AbilitySystemComponent->SetIsReplicated(true);

	// 3. 为能力系统组件设置复制模式为"混合复制模式（Mixed）"
	// 适配场景：当前角色是玩家控制的角色，需平衡逻辑权威与视觉/状态一致性
	// 核心逻辑：仅将影响角色核心逻辑的游戏玩法效果同步给角色的拥有者客户端（保证逻辑精准），
	// 而视觉提示（如技能特效、音效）和全局状态标签（如"中毒""无敌"）会同步给所有客户端（保证所有玩家看到一致的反馈）
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	// 4. 创建项目自定义的属性集组件（UAuraAttributeSet），命名为"AttributeSet"
	// 核心作用：集中存储和管理角色的可修改属性（如血量、蓝量、攻击力、防御力等），属性的变更会通过GAS系统自动协同同步，无需手动处理
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	// 设置Actor的网络更新频率为100次/秒（Hz）
	// 核心作用：控制服务器向客户端同步该Actor状态（如位置、血量等Replicated属性）的最大发送频率
	// 补充说明：
	// 1. 该值仅为"最大频率"，实际同步频率受网络状况、数据是否变更（脏标记）影响——仅当Actor有可同步属性变更时，才会按此频率发送数据
	// 2. 数值越高，同步越实时（如高速移动的角色、快节奏战斗的敌人），但会占用更多网络带宽；数值越低，带宽占用越少，但同步延迟可能增加
	// 3. 适用场景：常用于需要精准同步状态的Actor（如玩家角色、核心战斗单位），非关键Actor（如场景道具）可适当降低（如10-30Hz）以节省带宽
	// 4. 依赖Actor的bReplicates=true（可复制）属性生效，若bReplicates=false，该频率设置无效
	NetUpdateFrequency = 100.0f;

	
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
