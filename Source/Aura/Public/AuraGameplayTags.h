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
	
	FGameplayTag Attributes_Secondary_Armor;
protected:
	
private:
	static FAuraGameplayTags GameplayTags;
};

