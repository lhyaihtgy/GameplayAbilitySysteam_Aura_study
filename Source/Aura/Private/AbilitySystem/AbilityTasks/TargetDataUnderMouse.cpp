// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

//只要激活了技能，就会调用这个函数，在这个函数中，我们首先从能力系统中获取玩家操作器，然后获取当前光标下的碰撞结果，最后将碰撞结果的位置广播出去。
//只要有技能激活，这个函数就会被调用，光标的位置就会被广播出去，可以在VaildData中获取到光标位置
void UTargetDataUnderMouse::Activate()
{
	//从能力系统中获取玩家操作器
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	//获取当前光标下的碰撞结果
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);
	//将碰撞结果的位置广播出去
	ValidData.Broadcast(CursorHit.Location);
}
