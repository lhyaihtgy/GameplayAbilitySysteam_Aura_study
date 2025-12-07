// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEffectActor.h"


#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"


/**
 * @brief 构造函数：初始化Actor的核心组件与基础配置
 * @note 执行时机：Actor实例创建时（早于BeginPlay），仅初始化组件对象，不涉及场景交互逻辑
 * 核心逻辑：
 * 1. 禁用Actor的Tick更新（该Actor仅响应碰撞事件，无需帧更新，节省性能）；
 * 2. 创建场景根组件（SceneRoot）并设为根组件（原注释中“创建Mesh/Sphere组件”为未实现逻辑，代码仅初始化SceneRoot）；
 * 3. 组件层级：所有后续碰撞/渲染组件挂载到SceneRoot下
 */
AAuraEffectActor::AAuraEffectActor()
{
    // 禁用Actor的Tick更新（无帧更新需求，降低性能开销）
	PrimaryActorTick.bCanEverTick = false;
	
    // 创建场景根组件（作为所有子组件的父节点），并设为Actor的根组件
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

/**
 * @brief Actor初始化完成后的核心入口函数
 * @note 执行时机：Actor被生成且所有组件初始化完成后触发，晚于构造函数；
 *       核心用途：在此处初始化碰撞事件绑定、效果参数等场景交互逻辑（当前仅调用父类方法，暂无自定义逻辑）
 */
void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

/**
 * 给目标Actor应用指定的GameplayEffect（游戏效果）
 * @param TargetActor 要施加游戏效果的目标Actor（如玩家角色、敌人）
 * @param GamePlayEffectClass 要应用的游戏效果类模板（需继承自UGameplayEffect，如血量加成、减速效果蓝图类）
 * 核心逻辑：
 * 1. 通过GAS蓝图库获取目标的ASC（能力系统组件），判空避免非法访问；
 * 2. 创建效果上下文句柄（存储效果元数据）和规格句柄（存储效果配置）；
 * 3. 应用效果到目标，并缓存无限时长效果的活跃句柄（用于后续移除）；
 * 依赖说明：需确保目标Actor挂载ASC，否则效果无法应用
 */
void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GamePlayEffectClass)
{
	/********************************************************************
	【备选获取ASC的方式】通过接口判断Actor是否包含能力系统组件：
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(TargetActor);
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
	// - 存储效果的核心配置：效果类、物品效果等级（1.f）、上下文（EffectContextHandle）；
	// - 是“效果施加”的核心载体，后续可通过该句柄设置效果参数（如血量加成值、减速百分比）
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GamePlayEffectClass,ActorLevel,EffectContextHandle);
	
	// 6. 将效果规格应用到目标自身（ApplyGameplayEffectSpecToSelf）
	// EffectSpecHandle.Data.Get()：通过句柄获取底层的效果规格对象（需确保句柄有效，此处因前面判空+check，可安全访问）
	// 7. 获取效果应用后的活跃句柄，若该效果是无限时长且配置了“重叠结束时移除”，则缓存句柄，用于后续删除
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	
	// 判断当前效果是否为无限时长的GameplayEffect
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	// 若为无限时长效果，且配置了“重叠结束时移除”策略，则将活跃句柄和对应ASC缓存到Map中
	if (bIsInfinite && InfiniteEffectRemovePolicy == EffectRemovePolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}
}

/**
 * @brief 碰撞重叠触发函数：处理目标进入碰撞区域时的效果应用逻辑
 * @param TargetActor 进入碰撞区域的目标Actor（如玩家、敌人）
 * 核心逻辑：
 * 1. 分别判断瞬时/持续/无限三类效果的“应用策略”，若策略为“重叠时应用”，则给目标施加对应效果；
 * 2. 策略匹配规则：仅当效果的ApplycationPolicy为ApplyOnOverlap时，才执行ApplyEffectToTarget；
 * 注意点：无限效果若配置为“结束重叠时应用”，则不在此函数处理（移至OnEndOverlap）
 */
void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	// 瞬时效果：若策略为“重叠时应用”，则施加效果
	if (InstanceEffectApplycationPolicy == EffectApplycationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstanceGameplayEffectClass);
	}
	// 持续效果：若策略为“重叠时应用”，则施加效果
	if (DurationEffectApplycationPolicy == EffectApplycationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	// 无限效果：若策略为“重叠时应用”，则施加效果（此处原代码写为ApplyOnEndOverlap，属于笔误，需确认逻辑后修正）
	if (InfiniteEffectApplycationPolicy == EffectApplycationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

/**
 * @brief 碰撞结束重叠触发函数：处理目标离开碰撞区域时的效果应用与移除逻辑
 * @param TargetActor 离开碰撞区域的目标Actor（如玩家、敌人）
 * 核心逻辑：
 * 1. 效果应用：判断瞬时/持续/无限三类效果的“应用策略”，若策略为“结束重叠时应用”，则给目标施加对应效果；
 * 2. 效果移除：若无限效果配置了“结束重叠时移除”策略，则从目标ASC中删除该效果，并清理缓存的句柄；
 * 关键注意点：
 * - 遍历Map删除元素时，需先记录要删除的句柄，再批量移除（避免循环中修改Map导致迭代器失效）；
 * - 移除效果前需再次校验ASC有效性，防止目标Actor已销毁导致空指针
 */
void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	// 瞬时效果：若策略为“结束重叠时应用”，则施加效果
	if (InstanceEffectApplycationPolicy == EffectApplycationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstanceGameplayEffectClass);
	}
	// 持续效果：若策略为“结束重叠时应用”，则施加效果
	if (DurationEffectApplycationPolicy == EffectApplycationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	// 无限效果：若策略为“结束重叠时应用”，则施加效果
	if (InfiniteEffectApplycationPolicy == EffectApplycationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
	
	// 无限效果移除逻辑：若配置为“结束重叠时移除”，则删除目标身上的该效果
	if (InfiniteEffectRemovePolicy == EffectRemovePolicy::RemoveOnEndOverlap)
	{
		// 获取该目标的能力系统组件（二次校验，避免OnOverlap后ASC失效）
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;
		
		// 临时数组：记录需要从Map中移除的句柄（循环中不能直接修改Map，否则迭代器异常）
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		// 遍历缓存的无限效果句柄Map，找到当前目标ASC对应的所有效果句柄
		for (TPair<FActiveGameplayEffectHandle, UAbilitySystemComponent*>& HandlePair : ActiveEffectHandles)
		{
			// 匹配到当前目标的ASC，则移除该效果，并标记句柄待删除
			if (HandlePair.Value == TargetASC)
			{
				//参数1代表着，如果这个持续效果存在堆叠（存在多个给与该效果的actor），在离开触发一次end overlap的时候会减少一层堆叠，默认的-1
				//是直接删除所有堆叠，一就是删除一层
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key,1);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}
		
		// 批量删除Map中已移除的句柄，清理缓存
		for (auto& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}



