// Fill out your copyright notice in the Description page of Project Settings.
//CreateTargetDataUnderMouse 的核心作用是：通过 UE 提供的 NewAbilityTask 创建 UTargetDataUnderMouse 实例，并自动将该实例绑定到当前技能（UAuraProjectileSpell）的 Task 列表中；
//Ability Task 的核心机制：所有通过 “NewAbilityTask 创建的 Task 实例，在创建完成后立即自动”调用其 Activate() 虚函数（这是 UE 框架内置规则，无需手动调用）；
//最终效果：因为我在「技能激活的蓝图逻辑里」调用了 CreateTargetDataUnderMouse，所以 Task 实例创建、Activate () 执行、光标位置广播，会跟随 “技能激活” 同步发生，表现为 “激活技能后光标位置被广播”；
//Activate() 是广播的直接执行者：该函数内封装了 “获取光标下碰撞位置 + 触发 ValidData 广播” 的核心逻辑，是光标位置能被广播的根本原因。
#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

//这个宏声明了一个动态多播委托，名字是FMouseTargetDataSignature，它接受一个参数，类型是const FVector&，参数名是Data。这个委托可以在蓝图中绑定多个函数，当它被触发时，会调用所有绑定的函数，并传递一个FVector类型的数据作为参数。
//作用是广播当前光标的位置
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);
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
	void SendMouseCursorData();
	
	//这个函数用于处理服务端接收到客户端发送过来的光标数据后的回调逻辑。当服务端接收到客户端发送过来的光标数据时，这个函数会被触发
	//让绑定在ValidData上的函数能够获取到这些数据并进行相应的处理。
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,FGameplayTag ActivationTag);
};
