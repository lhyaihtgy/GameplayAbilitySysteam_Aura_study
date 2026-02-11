// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "UI/Widget/AuraUserWidget.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	//获取属性集
	UAuraAttributeSet* ASC = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	//检测原生属性信息是否已经生成
	check(AttributeInfo);
	
	//获取原生标签属性信息中的力量信息
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeinfoForTag(FAuraGameplayTags::Get().Attributes_Primary_Strength);
	Info.AttributeValue = ASC->GetStrength();//将这个信息中的力量值放入到Info中方便给后续广播给属性菜单
	AttributeInfoDelegate.Broadcast(Info);
}
