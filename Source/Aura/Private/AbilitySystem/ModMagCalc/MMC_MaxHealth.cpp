// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	//以下都是在配置VigorDef这个结构体，这个结构体的作用就是捕捉属性
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	//EGameplayEffectAttributeCaptureSource 用于指定从哪个对象捕捉属性，这里指定从目标对象捕捉属性,source就是施法者，Target就是被施法者
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	//这里是关闭快照机制的说法，这里需要知道的是这个类的作用就是捕捉属性，开启快照就是在GameplayEffectspec一创建
	//就马上去捕捉属性，而关闭快照就是等这个GaemeplayEffect被应用的时候才会去捕捉属性
	VigorDef.bSnapshot = false;
	
	//这里是将VigorDef这个结构体添加到RelevantAttributesToCapture这个数组中，这个数组的作用就是存储需要捕捉的属性
	RelevantAttributesToCapture.Add(VigorDef);
}

//这个函数被调用之后virgor变量(这个函数内部创建的)就会被赋值为目标对象上的活力(Virgor属性)，同时这个函数是一个自定义的属性计算函数，这个函数会根据活力属性和玩家的等级计算出最大生命值
//这也是为什么我们要实现战斗接口，使用战斗接口能够得到玩家的等级，这个函数对敌人依旧有效即使敌人的等级并不是在State类中而是在敌人角色类自身中因为获取等级这个操作是由战斗接口来完成的，敌人只需要正确实现战斗接口就行了
float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	//这里获取标签，可以获取source和Target的标签,这里的source就是指的施法者，Target就是指的被施法者
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	//这里是创建一个评估参数，这个参数的作用就是存储源和目标的标签
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	//这里是获取活力属性，这个函数的作用就是获取活力属性，并且返回活力属性的值
	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef,Spec,EvaluationParameters,Vigor);
	//防止出现活力值负数的情况
	Vigor = FMath::Max<float>(Vigor,0.f);
	
	//这里是通过Spec来获取这个游戏效果的施法者的UObject对象，再将这个施法者的U对象转化为战斗接口以得到施法者也就是玩家的等级
	ICombatInterface* CombatInterface =  Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();
	
	
	return 80.0f + Vigor*2.5f+10.0f * PlayerLevel;
}
