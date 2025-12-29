// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacter.generated.h"

class AAuraHUD;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
public:
	virtual void PossessedBy(AController* NewController) override;
	virtual  void OnRep_PlayerState() override;
	
	/** Combat Interface**/
	int32 GetPlayerLevel() override;
	/**  end Combat Interface **/
	
public:
	AAuraCharacter();
	
	
private:
	virtual void InitAbilitySystemInfo() override;
};
