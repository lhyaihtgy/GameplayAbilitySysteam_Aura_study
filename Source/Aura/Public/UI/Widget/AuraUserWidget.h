// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

/**
 * 
 */
/**
 * @brief 自定义UI控件基类，继承自UUserWidget，封装WidgetController的绑定逻辑
 * @note 所有需要关联WidgetController的UI控件都应继承此类，统一管理控制器的设置与事件触发
 * @warning WidgetController需在UI初始化前设置，否则可能导致UI数据同步异常
 */
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	// UE反射系统必备宏，自动生成类的反射元数据、构造函数等核心代码
	GENERATED_BODY()

public:
	/**
	 * @brief 设置当前UI对应的WidgetController（控制器）
	 * @param InWidgetController 待绑定的WidgetController对象（通常是自定义的UObject子类）
	 * @note 该函数可在蓝图中调用，用于动态绑定UI与业务逻辑控制器
	 * @details 1. 校验传入的控制器有效性 2. 赋值给AuraWidgetController 3. 触发WidgetControllerSet事件
	 */
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

	/**
	 * @brief 当前UI绑定的WidgetController对象（蓝图只读）
	 * @note 使用TObjectPtr智能指针，避免悬空指针，提升内存安全性
	 * @category 归类到"WidgetController"，方便蓝图中查找与管理
	 */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> AuraWidgetController;

protected:
	/**
	 * @brief WidgetController绑定完成后的蓝图可实现事件
	 * @note 1. 该函数无C++实现，需在蓝图子类中重写逻辑 2. 触发时机为SetWidgetController赋值完成后
	 * @details 常用于UI初始化（如绑定属性回调、刷新初始数据、注册事件监听）
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();	
};
