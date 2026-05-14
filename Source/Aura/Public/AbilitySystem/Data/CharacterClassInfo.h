// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayEffect;
/**
 * 
 */
//设定法师，战士，游侠三种职业
UENUM(BlueprintType)
enum class ECharacterClass:uint8
{
	Elementalist,
	Warrior,
	Ranger
};

//各个职业的初始默认属性
USTRUCT(BlueprintType)
struct FCharacterClassdefaultInfo
{
	GENERATED_BODY()
	
	//配置角色主要属性的游戏效果
	UPROPERTY(EditDefaultsOnly,Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;
};


//次要属性和关键属性在所有角色职业中是共享的
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	//一个职业对应一个主要属性
	UPROPERTY(EditDefaultsOnly,Category = "Character Class Defaults")
	TMap<ECharacterClass,FCharacterClassdefaultInfo> CharacterClassInfomation;
	
	//配置角色次要属性的游戏效果
	UPROPERTY(EditDefaultsOnly,Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> SendoryAttributes;
	//配置角色关键属性的游戏效果
	UPROPERTY(EditDefaultsOnly,Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> ValitaAttributes;
	
	//通过角色的职业得到这个职业的初始主要属性，后续是通过获取初始化主要属性的游戏效果来获取这个角色的主要属性
	FCharacterClassdefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);
};
