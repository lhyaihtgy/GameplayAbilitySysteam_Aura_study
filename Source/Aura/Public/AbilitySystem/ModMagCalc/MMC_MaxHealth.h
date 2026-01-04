// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxHealth.generated.h"
//这个类是专门用来计算最大生命值的类（获取角色的活力和等级来进行计算）
//但是这种自定义计算的类只能修改单个属性
//这个类能够捕捉属性，并且可以访问GameEffectSpec中的所有信息
/**
 * 
 */
UCLASS()
class AURA_API UMMC_MaxHealth : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UMMC_MaxHealth();
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	
private:
	
	FGameplayEffectAttributeCaptureDefinition VigorDef;
};
