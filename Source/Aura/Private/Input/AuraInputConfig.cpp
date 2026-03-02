// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const auto Action:AbilityInputActions)
	{
		if (Action.InputAction&& Action.Tag == InputTag)
		{
			return Action.InputAction;
		}
	}
	bLogNotFound = true;
	if (bLogNotFound)
	{
		UE_LOG(LogTemp,Error,TEXT("无法找到输入标签对应的能力输入动作，标签：[%s] 在InputConfig[%s]"),*InputTag.ToString(),*GetNameSafe(this));
	}
	return nullptr;
}
