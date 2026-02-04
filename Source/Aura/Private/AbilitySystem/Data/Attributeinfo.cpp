// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/Attributeinfo.h"


FAuraAttributeInfo UAttributeinfo::FindAttributeinfoForTag(const FGameplayTag& AttributeTag, bool bLognotFound) const
{
	for(FAuraAttributeInfo info:AttributeInfomation)
	{
		if (info.AttributeTag.MatchesTag(AttributeTag))
		{
			return info;
		}
	}
	
	if (!bLognotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("不能通过属性标签找到对应的属性信息，属性标签:[%s],类：[%S]"), *AttributeTag.ToString(),*GetNameSafe(this));
	}
	return FAuraAttributeInfo();
}
