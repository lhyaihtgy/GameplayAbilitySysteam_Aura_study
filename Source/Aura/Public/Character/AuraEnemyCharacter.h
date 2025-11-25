// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
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
	/** end enemy interface **/
protected:
	virtual void BeginPlay() override;
public:
	AAuraEnemyCharacter();
};


