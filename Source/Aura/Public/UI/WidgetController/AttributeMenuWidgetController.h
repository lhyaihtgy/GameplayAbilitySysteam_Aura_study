// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/Attributeinfo.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

struct FAuraAttributeInfo;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAuraAttributeInfo& , Info);

/**
 * 
 */
//这里完成下面的Blueprinttable声明因为 显式声明Blueprintable（允许作为蓝图父类）+ BlueprintType（允许作为蓝图变量类型），否则
//无法基于这个类创建蓝图
UCLASS(Blueprintable,BlueprintType) 
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
protected:
	
	//这个对象中保存有C++原生标签
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeinfo> AttributeInfo;
public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;
	
	//这是这个控件的一个动态多播委托可以让多个widget控件绑定这个委托对这个委托做出反应
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;
};
