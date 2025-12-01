// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

struct FOnAttributeChangeData;
// 声明「血量变化」的动态多播委托（带1个浮点型参数）
// 动态多播委托：支持C++和蓝图双向绑定/触发，可同时绑定多个回调函数
// 参数说明：float NewHealth → 血量变化后的最新血量值
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature,float,NewHealth);

// 声明「最大血量变化」的动态多播委托（带1个浮点型参数）
// 参数说明：float NewMaxHealth → 最大血量变化后的最新上限值
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSignature,float,NewMaxHealth);

//和上面一致只不过是蓝量的
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaChangedSignature,float,NewMana);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxManaChangedSignature,float,NewMaxMana);

/**
 * Overlay UI（血条/蓝条面板）的逻辑控制器
 * 负责属性数据的监听、计算，并通过委托通知UI更新显示
 */
UCLASS(Blueprintable,BlueprintType)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	
	//广播初始值
	virtual void BroadcastInitialValues() override;
	
	virtual void BindCallbacksToDependencies() override;
	
	/**
	 * 【血量变化委托】- 蓝图可绑定
	 * 用途：当玩家血量发生变化时，触发该委托通知UI更新血条显示
	 * BlueprintAssignable：允许在蓝图中绑定该委托的事件（比如UI蓝图里绑定“更新血条进度”逻辑）
	 * Category = "GAS|Attributes"：在蓝图细节面板中归类，方便查找（GAS=能力系统，Attributes=属性）
	 */
	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnHealthChangedSignature OnHealthChanged;
	
	/**
	 * 【最大血量变化委托】- 蓝图可绑定
	 * 用途：当玩家最大血量上限变化时（比如升级、加buff），触发该委托通知UI更新血条上限显示
	 * BlueprintAssignable：支持蓝图中绑定回调（比如UI蓝图里调整血条总长度、显示最大血量数值）
	 * Category = "GAS|Attributes"：归类到GAS属性模块，和其他属性委托统一管理
	 */
	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnMaxHealthChangedSignature OnMaxHealthChanged;
	
	//和上面一致只不过是蓝量的
	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnManaChangedSignature OnManaChanged;
	
	//和上面一致只不过是蓝量的
	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnMaxManaChangedSignature OnMaxManaChanged;
protected:
	//以下两个函数是当生命值和最大生命值改变的时候会被调用的函数
	void HealthChanged(const FOnAttributeChangeData& Data) const;
	void MaxHealthChanged(const FOnAttributeChangeData& Data) const;
	
	//以下两个函数时当魔力值和最大魔力值发生变化的时候会被调用的函数
	void ManaChanged(const FOnAttributeChangeData& Data) const;
	void MaxManaChanged(const FOnAttributeChangeData& Data) const;
};
