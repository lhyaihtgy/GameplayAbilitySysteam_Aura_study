// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemyCharacter.generated.h"

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
};


