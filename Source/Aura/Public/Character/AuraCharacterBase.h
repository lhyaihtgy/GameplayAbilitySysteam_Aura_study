// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "AuraCharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
UCLASS(ABSTRACT)
class AURA_API AAuraCharacterBase : public ACharacter,public IAbilitySystemInterface 
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAuraCharacterBase();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const{return AttributeSet;}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, category = "combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	
	//敌人的能力系统组件
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySysteamComponent;
	
	//敌人的属性集组件
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};
