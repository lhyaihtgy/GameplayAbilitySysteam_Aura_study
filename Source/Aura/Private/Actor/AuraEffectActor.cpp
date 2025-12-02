// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEffectActor.h"


#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"


/**
 * @brief 构造函数：初始化Actor的核心组件与基础配置
 * @note 执行时机：Actor实例创建时（早于BeginPlay），仅初始化组件对象，不涉及场景交互逻辑
 * 核心逻辑：创建静态网格组件（Mesh）和球形碰撞组件（Sphere），并建立组件层级关系
 */
AAuraEffectActor::AAuraEffectActor()
{
 	// 禁用Actor的Tick更新（该Actor仅响应碰撞事件，无需帧更新，节省性能）
	PrimaryActorTick.bCanEverTick = false;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
	
}





/**
 * @brief Actor初始化完成后的核心入口函数
 * @note 执行时机：Actor被生成且所有组件初始化完成后触发，晚于构造函数
 */
void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

/**
 * 给目标Actor应用指定的GameplayEffect（游戏效果）
 * @param Target 要施加游戏效果的目标Actor（如玩家角色、敌人）
 * @param GamePlayEffectClass 要应用的游戏效果类模板（需继承自UGameplayEffect，如血量加成、减速效果蓝图类）
 * 核心逻辑：通过GAS框架给目标的ASC（能力系统组件）施加效果，依赖句柄管理GAS核心对象的生命周期与安全访问
 */
void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GamePlayEffectClass)
{
	/********************************************************************
	【备选获取ASC的方式】通过接口判断Actor是否包含能力系统组件：
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Target);
	if (ASCInterface == nullptr)
		return;
	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
	说明：并非所有Actor都挂载ASC，需通过接口/蓝图库函数判断，避免空指针
	********************************************************************/
	
	// 1. 通过GAS蓝图库获取目标Actor的AbilitySystemComponent（ASC）——GAS核心组件，所有效果/技能都通过它管理
	// ASC是连接Actor和GAS框架的桥梁，只有获取到有效ASC，才能施加游戏效果
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	// 判空：若目标无ASC（如场景静态Actor），直接返回，避免后续非法访问
	if (TargetASC == nullptr) return;
	
	// 2. 强制检查：确保传入的游戏效果类模板非空（若为空，后续创建效果规格会崩溃）
	// check断言在Debug模式下触发，提示开发者配置效果类，Release模式下等价于空检查
	check(GamePlayEffectClass);
	
	// 3. 创建游戏效果上下文句柄（FGameplayEffectContextHandle）
	// 【句柄核心作用】：
	// - 管理FGameplayEffectContext（游戏效果上下文）的生命周期，避免悬空指针；
	// - 存储效果的元数据（发起者、目标、触发场景、效果归因等），是GAS效果的“上下文标签”；
	// - 句柄是UE对UObject/结构体的安全封装，自动处理内存释放，比直接操作上下文对象更安全
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	
	// 4. 给上下文设置“源对象”（当前EffectActor）——标记效果的发起者，便于后续追溯效果来源（如哪个机关触发了加血）
	EffectContextHandle.AddSourceObject(this);
	
	// 5. 创建游戏效果规格句柄（FGameplayEffectSpecHandle）
	// 【句柄核心作用】：
	// - 管理FGameplayEffectSpec（游戏效果规格）的生命周期，安全访问规格对象；
	// - 存储效果的核心配置：效果类、效果等级（1.f）、上下文（EffectContextHandle）；
	// - 是“效果施加”的核心载体，后续可通过该句柄设置效果参数（如血量加成值、减速百分比）
	FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GamePlayEffectClass,1.f,EffectContextHandle);
	
	// 6. 将效果规格应用到目标自身（ApplyGameplayEffectSpecToSelf）
	// EffectSpecHandle.Data.Get()：通过句柄获取底层的效果规格对象（需确保句柄有效，此处因前面判空+check，可安全访问）
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}



