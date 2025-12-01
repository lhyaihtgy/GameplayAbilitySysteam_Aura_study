// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraWidgetController.generated.h"

class UAuraAttributeSet;
class UAttributeSet;
class UAbilitySystemComponent;
/**
 * @brief UI业务逻辑控制器核心类，作为Model与UserWidget之间的中间层（MVVM架构中的ViewModel角色）
 * @details 核心职责：
 * 1. 从数据层（Model，如AttributeSet/GameState等存储数据的模块）读取原始数据；
 * 2. 对原始数据做业务逻辑处理（如数值格式化、状态转换、权限校验）；
 * 3. 为UI层（UserWidget）提供统一的数据访问接口，解耦UI与底层数据；
 * 
 * 依赖关系设计（单向依赖，保证低耦合）：
 * - 本类 → Model层：主动依赖并读取Model数据，无需感知Model内部实现；
 * - UserWidget → 本类：UI仅从绑定的Controller获取处理后的数据，无需直接访问Model；
 * - 本类 → UserWidget：完全无感知，不依赖任何具体Widget类，仅提供数据接口；
 * 
 * @note 每个UI组件可绑定独立的Controller实例，也可共享全局Controller（如玩家状态控制器）
 * @warning 需在UI初始化前完成Controller的初始化（绑定PlayerController/AttributeSet等核心数据），否则UI会获取空数据
 */
USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()
	
	FWidgetControllerParams(){}
	FWidgetControllerParams(APlayerController* PC,APlayerState* PS,UAbilitySystemComponent* USC,UAttributeSet* AS)
		:PlayerController(PC),PlayerState(PS),AbilitySystemComponent(USC),AttributSet(AS)
	{}
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributSet = nullptr;	
};

UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	// UE反射系统必备宏，自动生成类的反射元数据、构造/析构等核心逻辑
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& params);
	
	virtual void BroadcastInitialValues();
	
	virtual void BindCallbacksToDependencies();
protected:
	/**
	 * @brief 关联的玩家控制器（客户端视角的玩家核心控制类）
	 * @note 用于获取玩家输入、视角、本地玩家信息等基础数据，BlueprintReadOnly保证蓝图仅可读，避免UI直接修改控制器状态
	 * @see APlayerController
	 */
	UPROPERTY(BlueprintReadOnly, Category="WidgetController|Player")
	TObjectPtr<APlayerController> PlayerController;

	/**
	 * @brief 关联的玩家状态类（存储玩家全局状态：等级、血量、金币等）
	 * @note 跨客户端/服务器同步的核心数据载体，是Model层的核心依赖之一
	 * @see APlayerState
	 */
	UPROPERTY(BlueprintReadOnly, Category="WidgetController|Player")
	TObjectPtr<APlayerState> PlayerState;

	/**
	 * @brief 关联的技能系统组件（GAS核心组件，处理技能激活、GameplayEffect等）
	 * @note 用于读取/处理技能相关数据（如冷却、消耗、技能状态）
	 * @see UAbilitySystemComponent
	 */
	UPROPERTY(BlueprintReadOnly, Category="WidgetController|GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	/**
	 * @brief 关联的属性集（存储玩家核心属性：生命值、法力值、攻击力等）
	 * @note Model层核心数据载体，本类的主要数据来源，所有UI属性均从这里读取并处理
	 * @see UAttributeSet
	 */
	UPROPERTY(BlueprintReadOnly, Category="WidgetController|GAS")
	TObjectPtr<UAttributeSet> AttributeSet;
};
