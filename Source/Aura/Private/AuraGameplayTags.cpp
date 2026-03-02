// Fill out your copyright notice in the Description page of Project Settings.


/***
 *这是C++原生的标签结构体，在这个结构提内部的标签可以天然给C++使用，也可以给蓝图使用
 ****/
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

//这个函数用来初始化创建原生C++标签
void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	//一级属性标签定义和添加
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"),
		FString("增加角色的物理伤害")
		);
	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"),
		FString("增加角色的法术威力")
		);
	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"),
		FString("增加角色的护甲和护甲穿透")
		);
	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"),
		FString("增加角色的生命值")
		);
	
	
	
	//二级属性标签定义和添加
	//储存Armor这个Tag在我的标签管理类中
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"),
		FString("减少受到伤害的机率，提升格挡机率")
		);
	
	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"),
		FString("无视敌方一定比例的护甲，提升暴击几率")
		);
	
	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.BlockChance"),
		FString("有几率将受到的伤害减半")
		);
	
	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"),
		FString("有几率使伤害翻倍并附加暴击加成")
		);
	
	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamage"),
		FString("触发暴击时附加的额外伤害")
		);
	
	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitResistanc"),
		FString("降低敌方攻击我方角色时的暴击几率")
		);
	
	GameplayTags.Attributes_Secondary_HealthRegeneration= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.HealthRegeneration"),
		FString("每1秒恢复的生命值量")
		);
	
	GameplayTags.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ManaRegeneration"),
		FString("每1秒恢复的魔力量")
		);
	
	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"),
		FString("角色最大生命值")
		);
	
	GameplayTags.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxMana"),
		FString("角色最大魔力量")
		);
	
	
	//输入标签定义和添加
	//储存输入标签这个Tag在我的标签管理类中
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.LMB"),
		FString("鼠标左键输入标签")
		);
	

	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.RMB"),
		FString("鼠标右键输入标签")
		);
	
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.1"),
		FString("数字按键1输入标签")
		);
	
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.2"),
		FString("数字按键2输入标签")
		);
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.3"),
		FString("数字按键3输入标签")
		);
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.4"),
		FString("数字按键4输入标签")
		);
}

FAuraGameplayTags FAuraGameplayTags::GameplayTags;


