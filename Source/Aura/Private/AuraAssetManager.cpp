// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraAssetManager.h"

#include "AuraGameplayTags.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

//获得这个管理类的唯一实例
UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);
	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

//这个函数很早的时候就会被调用
void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	
	FAuraGameplayTags::InitializeNativeGameplayTags();
	
	//这行代码的作用是调用AbilitySystemGlobals单例对象的InitGlobalData函数，来初始化全局的能力系统数据，
	//这些数据包括一些全局的GameplayTag、GameplayAttribute等，这些数据在能力系统中是非常重要的，必须在使用能力系统之前进行初始化。
	UAbilitySystemGlobals::Get().InitGlobalData();
}
