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
	//储存Armor这个Tag在我的标签管理类中
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Armor"),FString("减少受到伤害的机率，提升格挡机率"));
	
}

FAuraGameplayTags FAuraGameplayTags::GameplayTags;


