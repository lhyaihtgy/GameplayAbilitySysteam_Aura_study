// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "UI/Widget/AuraUserWidget.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	//获取属性集
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);
	for (auto& Pair:AS->TagsToAttributes)
	{
		//GetGameplayAttributeValueChangeDelegate(Pair.Value())：
		//根据传入的游戏属性（比如生命值），获取该属性的值变更委托（Delegate） —— 这个委托的作用是 “监听” 属性值的变化。
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this,Pair](const FOnAttributeChangeData Data)
		{
				//当属性值变化的时候上面的时候，widget部件就会触发这个lambda，然后这个lambda就会触发widget的属性更新委托将变化后的值进行广播
				//FAuraAttributeInfo Info = AttributeInfo->FindAttributeinfoForTag(Pair.Key);
				//Info.AttributeValue = Pair.Value().GetNumericValue(AS);
				//AttributeInfoDelegate.Broadcast(Info);
				//将以上三行整合为了一个函数
				BroadcastAttributeInfo(Pair.Key,Pair.Value());
		}
	);
	}
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	//获取属性集
	UAuraAttributeSet* ASC = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	//检测原生属性信息是否已经生成
	check(AttributeInfo);
	
	for (auto& pair:ASC->TagsToAttributes)
	{
		//得到该属性标签的结构体
		//FAuraAttributeInfo Info = AttributeInfo->FindAttributeinfoForTag(pair.Key);
		//这个属性访问器是一个静态的函数，需要传入具体的属性集，才知道你要去哪一个属性集中拿属性
		//Info.AttributeValue = pair.Value().GetNumericValue(ASC);
		//将该属性进行广播
		//AttributeInfoDelegate.Broadcast(Info);
		BroadcastAttributeInfo(pair.Key,pair.Value());
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute) const
{
	//得到该属性标签的结构体
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeinfoForTag(AttributeTag);
	//这个属性访问器是一个静态的函数，需要传入具体的属性集，才知道你要去哪一个属性集中拿属性
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	//将该属性进行广播
	AttributeInfoDelegate.Broadcast(Info);
}
