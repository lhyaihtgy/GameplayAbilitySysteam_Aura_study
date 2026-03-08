// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssertTags, const FGameplayTagContainer&/*Assert Tags*/)

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void AbilityActorInfoSet();
	
	FEffectAssertTags EffectAssertTags;
	
	//增加角色的技能到技能系统组件中
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StarupAbilities);
	 
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
protected:
	//将下面的这个函数设置为远程RPC是因为，现在遇到的bug是客户端捡起血瓶都道具回血的时候没有显示图画文字，这个文字是在下面这个函数中被初始化的
	//这个函数在后面被设置为了AbilityActorInfoSet这个委托的回调，这个委托i是当游戏效果应用到自身时，回调函数会在服务端被调用，客户端不会进行调用，所以需要将这个函数设置为远程RPC，然后函数会在服务端调用，在客户端进行执行
	UFUNCTION(CLIENT, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayEffectSpec& EffectSpec,FActiveGameplayEffectHandle ActiveEffectHandle);
};
