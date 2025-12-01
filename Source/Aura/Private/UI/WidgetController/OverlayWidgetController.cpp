// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AttributeSet.h"
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
