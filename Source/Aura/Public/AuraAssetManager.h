// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
//还需要去到这个项目的config文件中将这个类设置为这个项目的资源管理器，去config文件夹下修改DefaultEngine.ini文件
//这个类的作用是用来管理资产的，而C++创建的标签就是一个资产，这个类现在管理：C++创建的标签(AuraGameplayTags)
#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AuraAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	//这个函数用于让外部获取到我这个这个管理类对象（单例模式，这个类对象只会实例化一次）
	static UAuraAssetManager& Get();
protected:
	virtual void StartInitialLoading() override;
};
