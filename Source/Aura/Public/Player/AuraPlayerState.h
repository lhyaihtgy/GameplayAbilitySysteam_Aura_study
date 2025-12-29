// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UabilitySystemComponent;
class UAttributeSet;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState,public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const{return AttributeSet;}
	
	FORCEINLINE int32 GetPlayerLevel() {return Level;}
protected:
	
	//玩家的能力系统组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent>	AbilitySystemComponent;
	
	//玩家的属性集组件
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
	//角色的等级，这个属性需要可以被复制到服务器中所以需要进行复制属性必须要进行的操作
	UPROPERTY(VisibleAnywhere,ReplicatedUsing=OnRep_Level)
	int32 Level = 1;
private:
	//复制属性必须要进行的操作
	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
};
