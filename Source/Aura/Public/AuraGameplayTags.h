// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"

//描述标签的实例，可以认为就是C++创建的标签，可以被C++使用，创建时机要尽可能的早

/**
 * 
 */
struct FAuraGameplayTags
{
public:
	static const FAuraGameplayTags& Get(){return GameplayTags;}
	static void InitializeNativeGameplayTags();
	
	//一级属性标签
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;
	
	//二级属性标签
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
	
	//左键鼠标输入标签
	FGameplayTag InputTag_LMB;
	//右键鼠标输入标签
	FGameplayTag InputTag_RMB;
	
	//同上数字键的输入标签
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
private:
	static FAuraGameplayTags GameplayTags;
};

