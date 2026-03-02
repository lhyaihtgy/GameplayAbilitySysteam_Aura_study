// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayAbility;
UCLASS(ABSTRACT)
class AURA_API AAuraCharacterBase : public ACharacter,public IAbilitySystemInterface,public ICombatInterface
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
	
	virtual void InitAbilitySystemInfo();
	
	//这个GE效果用于初始化重要属性初始生命值和魔力值
	UPROPERTY(BlueprintReadOnly,EditAnywhere,category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
	
	//这个GE效果用于在游戏开始时初始化主要属性
	UPROPERTY(BlueprintReadOnly,EditAnywhere,category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
	
	//初始化二级属性
	UPROPERTY(BlueprintReadOnly,EditAnywhere,category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;
	
	
	/*可以将下面的两个函数变成一个函数让一个函数完成初始化的全部工
	//这个函数用于实现初始化主要属性
	void InitializePrimaryAttributes() const;
	//初始化二级属性使用
	void InitializeSecondaryAttributes() const;
	*/
	//下面这个函数
	void InitializeDefaultAttributes() const;
	
	//这个函数用于帮助上面这个函数完成初始化主要和二级属性
	void ApplyEffectToself(TSubclassOf<UGameplayEffect> GameplayEffect,float level) const;
	
	//这个函数的作用是往技能容器中增加技能，并且只能在服务端增加技能，客户端不允许往这个容器中增加技能
	void AddCharacterAbilities();
private:
	//角色在游戏一开始就会拥有的能力
	UPROPERTY(EditAnywhere,Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
};
