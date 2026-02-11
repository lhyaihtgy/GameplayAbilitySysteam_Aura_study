// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Attributeinfo.generated.h"

/**
 * 
 */
//以下是一个描述属性信息的结构体,当这个属性信息发生变化的时候会被广播到widegt中
USTRUCT(BlueprintType)
struct FAuraAttributeInfo
{
	GENERATED_BODY()
	//属性标签
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag();
	
	//Ftext就是显示到用户界面用户能够看到的界面文字
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName = FText();
	
	//属性名字
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeDescription = FText();
	
	//属性具体的值
	UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.0f;
	// ReSharper disable once UnrealHeaderToolError
};

UCLASS()
class AURA_API UAttributeinfo : public UDataAsset
{
	GENERATED_BODY()
public:
	
	//通过属性标签获得属性信息结构体
	FAuraAttributeInfo FindAttributeinfoForTag(const FGameplayTag& tag,bool bLognotFound = false) const;
	
	//用来保存属性信息的集合
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraAttributeInfo> AttributeInfomation;
};
