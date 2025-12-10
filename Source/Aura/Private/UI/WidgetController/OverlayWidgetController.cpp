// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"


/**
 * 广播属性初始值（核心作用：UI首次加载时，显示初始的血量/最大血量，避免UI空白）
 * 执行时机：WidgetController初始化完成后立即调用，给UI“填初始值”
 */
void UOverlayWidgetController::BroadcastInitialValues()
{
	// 强制转换AttributeSet为自定义的AuraAttributeSet（GAS属性集）
	// CastChecked：转换失败时触发断言（方便调试），确保AttributeSet是预期的类型（非空且是AuraAttributeSet）
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	// 广播初始血量值 → 触发UI更新（比如血条显示初始血量）
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	// 广播初始最大血量值 → 触发UI更新（比如血条的最大长度匹配初始最大血量）
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	
	//和上面类似
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

/**
 * 绑定属性变化的回调到GAS（游戏能力系统）依赖项
 * 核心作用：监听GAS中血量/最大血量的属性变化，注册“属性变了要执行的回调函数”
 * 执行时机：WidgetController初始化时（早于BroadcastInitialValues），仅需注册一次
 */
void UOverlayWidgetController::BindCallbacksToDependencies()
{
	// 转换为自定义属性集（const修饰：仅读取属性，不修改）
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	// 1. 绑定“当前血量”属性变化的回调：
	// GetGameplayAttributeValueChangeDelegate：GAS提供的接口，获取指定属性（血量）的变化委托
	// AddUObject：将“血量变化时要执行的函数（HealthChanged）”绑定到该委托，关联当前WidgetController实例
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()
		).AddUObject(this,&UOverlayWidgetController::HealthChanged);
	
	// 2. 绑定“最大血量”属性变化的回调：逻辑同上，监听最大血量属性变化
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()
		).AddUObject(this,&UOverlayWidgetController::MaxHealthChanged);
	
	//和上面一样只不过时魔力的
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()
		).AddUObject(this,&UOverlayWidgetController::ManaChanged);
	
	//和上面一致
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()
		).AddUObject(this,&UOverlayWidgetController::ManaChanged);
	
	
	
	//函数介绍
	/**
 * @brief 判断当前Gameplay标签是否与目标标签匹配（支持层级标签的包含性匹配）
 * 
 * @param MessageTag 待比较的目标FGameplayTag（要检查当前标签是否与该标签匹配）
 * @return bool 匹配结果：true表示当前标签与目标标签匹配；false表示不匹配
 * 
 * @note 匹配规则基于Gameplay标签的层级结构（用"."分隔的父子关系）：
 *       1. 精确匹配：当前标签与目标标签完全相同（如"Ability.Attack" == "Ability.Attack"）；
 *       2. 父标签匹配：当前标签是目标标签的"父标签"（如"Ability" 匹配 "Ability.Attack"，因为"Ability"是"Ability.Attack"的顶层父标签）；
 *       3. 子标签不匹配：当前标签是目标标签的"子标签"时不匹配（如"Ability.Attack" 不匹配 "Ability"，子标签无法匹配父标签）。
 * @see FGameplayTag的层级结构：标签通过"."分隔表示父子关系（如"Root.Parent.Child"中，Root是Parent的父标签，Parent是Child的父标签）
 * @usage 常用于判断技能/状态标签是否符合条件（如检查某个技能标签是否属于"Ability.Attack"类型，或某个状态是否属于"State.Debuff"类别）
 */
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssertTags.AddLambda(
		[this](const FGameplayTagContainer& AssertTags)	
		{
			for (FGameplayTag Tag: AssertTags)
			{
				//DOTO将标签广播到widget Controller
				
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("message"));
				//
				if (Tag.MatchesTag(MessageTag))
				{
					//在数据表中找到这个Tag对应的这一行
					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable,Tag);
					//将这一行广播给小组件
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		}
		);
}

/**
 * 血量属性变化的回调函数（由GAS自动触发）
 * @param Data GAS传递的属性变化数据（包含旧值、新值、变化原因等）
 * 执行时机：GAS中“当前血量”属性被修改时（比如受击掉血、回血、死亡）
 */
void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	// 广播最新血量值 → 通知UI更新（比如血条进度、血量数字显示）
	OnHealthChanged.Broadcast(Data.NewValue);
}

/**
 * 最大血量属性变化的回调函数（由GAS自动触发）
 * @param Data GAS传递的属性变化数据
 * 执行时机：GAS中“最大血量”属性被修改时（比如升级、加buff、装备加成）
 */
void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	// 广播最新最大血量值 → 通知UI更新（比如血条总长度、最大血量数字显示）
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

//和上面一致，只不过是魔力
void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChanged.Broadcast(Data.NewValue);
}

//和上面一致只不过是魔力
void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}
