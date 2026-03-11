// Fill out your copyright notice in the Description page of Project Settings.
//这个类是一个继承自UAbilityTask的类，名字是UTargetDataUnderMouse。它定义了一个静态函数CreateTargetDataUnderMouse，用于创建这个任务的实例，并且有一个动态多播委托ValidData，
//用于广播当前光标的位置。这个类可以在蓝图中使用，允许玩家在游戏中通过鼠标点击来获取目标数据。
#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

//这个宏声明了一个动态多播委托，名字是FMouseTargetDataSignature，它接受一个参数，类型是const FVector&，参数名是Data。这个委托可以在蓝图中绑定多个函数，当它被触发时，会调用所有绑定的函数，并传递一个FVector类型的数据作为参数。
//作用是广播当前光标的位置
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FVector&, Data);
/**
 * 
 */
UCLASS()
class AURA_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()
	public:
	//Meta是UE4中用来修饰函数的属性，BlueprintCallable表示这个函数可以在蓝图中被调用，Category是蓝图中显示的分类，DisplayName是蓝图中显示的函数名称，HidePin表示隐藏某个参数的输入引脚，DefaultToSelf表示默认将OwningAbility参数设置为调用该函数的对象，BlueprintInternalUseOnly表示这个函数只能在蓝图内部使用。
	UFUNCTION(BlueprintCallable,Category = "Ability|Tasks",meta= (DisplayName = "TargetDataUnderMouse",HidePin = "OwningAbility", DefaultToSelf = "OwningAbility",BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);
	
	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;
	
private:
	virtual void Activate() override;
};
