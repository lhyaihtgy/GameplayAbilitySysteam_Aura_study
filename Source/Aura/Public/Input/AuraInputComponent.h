// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	template<class UserClass,typename PressedFuncType,typename ReleasedFuncType,typename HeldFuncType>
	void BindAbilityAction(const UAuraInputConfig* InputConfig,UserClass* Object,PressedFuncType PressedFunc,ReleasedFuncType ReleasedFunc,HeldFuncType HeldFunc);
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityAction(const UAuraInputConfig* InputConfig, UserClass* Object,PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);
	for (const FAuaInputAction&Action:InputConfig->AbilityInputActions)
	{
		if (Action.InputAction&& Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc,Action.InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc,Action.InputTag);
			}
			if (HeldFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc,Action.InputTag);
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,FString::Printf(TEXT("输入配置数据资产[%s]中存在无效的输入动作或输入标签，请检查AbilityInputActions数组"),*GetNameSafe(InputConfig)));
		}
	}
}
