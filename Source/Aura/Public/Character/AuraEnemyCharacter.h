// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemyCharacter.generated.h"

class UWidgetComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemyCharacter : public AAuraCharacterBase,public IEnemyInterface
{
	GENERATED_BODY()
public:
	/** enemy Interfacce**/
	virtual void HightLightEnemy() override;
	virtual void UnHightLightEnemy() override;
	/** end enemy Interface **/
	
	/** Combat Interface**/
	int32 GetPlayerLevel() override;
	/**  end Combat Interface **/
	
	/**
	 * 下面是两个委托，在蓝图中进行定义，会在生命值发生变化的时候发出广播
	 */
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilitySystemInfo() override;
public:
	AAuraEnemyCharacter();
protected:
	//敌人的等级，因为敌人是ai，对于ai的操作都是在服务端进行的,包括ai敌人的值计算
	//所以这个属性不需要复制到客户端所以敌人的等级不需要进行复制
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Character Class Defaults")
	int32 Level;
	
	//敌人的血条
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;
};


