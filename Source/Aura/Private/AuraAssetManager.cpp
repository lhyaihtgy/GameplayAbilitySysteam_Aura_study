// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraAssetManager.h"

#include "AuraGameplayTags.h"

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
}
