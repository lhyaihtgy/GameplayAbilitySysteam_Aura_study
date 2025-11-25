// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraEnemyCharacter.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
AAuraEnemyCharacter::AAuraEnemyCharacter()
{
	// 设置角色网格体（Mesh）对“可见性碰撞通道（ECC_Visibility）”的碰撞响应为“阻挡（ECR_Block）”
	// 核心作用：让该网格体在“可见性通道”的碰撞检测中被判定为“阻挡物”
	// 常见场景：配合鼠标选中检测（如GetHitResultUnderCursor），确保鼠标能“命中”该角色/物体
	// 补充说明：
	// - ECC_Visibility（可见性通道）：UE预设碰撞通道，专门用于UI交互、物体选中、视线检测等场景
	// - ECR_Block（阻挡响应）：碰撞时会阻挡该通道的检测射线/碰撞体，使检测函数能识别到该物体
	// 若需取消选中检测，可改为ECR_Ignore（忽略）；若需仅检测不阻挡，可改为ECR_Overlap（重叠）
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
	
	// 1. 创建Aura自定义的能力系统组件（UAurabilitySystemComponent），命名为"AbilitySysteam"
	// 核心作用：作为GAS（Gameplay Ability System）的核心载体，负责管理角色的能力（Abilities）、游戏玩法效果（Gameplay Effects）、属性（AttributeSet）等核心逻辑
	AbilitySysteamComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySysteamComponent");

	// 2. 设置能力系统组件为"可复制"
	// 关键作用：启用组件的网络同步功能，让服务器端的能力相关状态（如技能触发、效果生效）能按复制模式同步到客户端，多人游戏必需配置
	AbilitySysteamComponent->SetIsReplicated(true);
	
	// 3. 为能力系统组件设置复制模式为"最小复制模式（Minimal）"
	// 适配场景：当前角色是AI控制的角色（非玩家控制），无需同步AI的内部能力逻辑（如AI的buff/debuff计算），仅同步必要的视觉提示（特效/音效）和全局标签（如"狂暴""中毒"）
	// 核心价值：减少网络带宽冗余消耗，同时保证所有客户端能看到AI的视觉反馈和状态标签，平衡性能与体验一致性
	AbilitySysteamComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	// 4. 创建Aura自定义的属性集组件（UAuraAttributeSet），命名为"AttributeSet"
	// 核心作用：集中存储角色的可修改属性（如血量、蓝量、攻击力、防御力），由能力系统组件统一管理，属性变化会通过GAS自动同步
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	
}
void AAuraEnemyCharacter::HightLightEnemy()
{
	
	// 启用角色网格体的“自定义深度渲染”功能
	// 开启后，该网格体的深度信息会被写入独立的自定义深度缓冲区，用于后续后期处理（如描边、高亮）
	GetMesh()->SetRenderCustomDepth(true);

	// 设置角色网格体的“自定义深度模板值”为红色通道标识
	// 模板值用于后期处理时“精准识别该物体”，配合后处理材质（如仅对模板值为CUSTOM_DEPTH_RED的物体应用高亮）
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

	// 启用武器的“自定义深度渲染”功能
	// 使武器的深度信息也被纳入自定义深度缓冲区，支持和角色统一的后期处理逻辑
	Weapon->SetRenderCustomDepth(true);
	
	// 设置武器的“自定义深度模板值”为红色通道标识
	// 让武器和角色使用相同的模板值，确保后期处理时（如高亮）能同时作用于角色和武器
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	
	
	
}

void AAuraEnemyCharacter::UnHightLightEnemy()
{
	
	//关闭角色网格体的“自定义深度渲染”功能
	GetMesh()->SetRenderCustomDepth(false);
	
	//关闭武器的“自定义深度渲染”功能
	Weapon->SetCustomDepthStencilValue(false);
}

void AAuraEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();	
	if (AbilitySysteamComponent)
	{
		//初始化能力组件的拥有者和生效者都是该对象自身
		AbilitySysteamComponent->InitAbilityActorInfo(this,this);
	}
}
