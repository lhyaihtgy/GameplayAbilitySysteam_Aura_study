// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"



class UGameplayEffect;

UENUM(BlueprintType)
enum class EffectApplycationPolicy
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EffectRemovePolicy
{
	RemoveOnEndOverlap,
	DoNotRemove
};
UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	

	AAuraEffectActor();

protected:
	virtual void BeginPlay() override;
	
	
	//效果产生后是否移除产生效果的actor
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effects")
	bool bDestoryOnEffectRemoval = false;
	
	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor,TSubclassOf<UGameplayEffect> GamePlayEffectClass);
	
	//处理和血瓶等产生重叠的逻辑
	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);
	
	//处理和血瓶等重叠结束的逻辑
	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);
	
	//这是一个用于实现即时游戏效果的类
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InstanceGameplayEffectClass;
	//即时效果的处理策略
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EffectApplycationPolicy InstanceEffectApplycationPolicy = EffectApplycationPolicy::DoNotApply;
	
	//这是一个具有短时间持续效果的类
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;
	//短暂效果的处理策略
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EffectApplycationPolicy DurationEffectApplycationPolicy = EffectApplycationPolicy::DoNotApply;
	
	//这是一个具有无限持续效果的类
    UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Applied Effects")
    TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
	//无限时间效果的处理策略
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EffectApplycationPolicy InfiniteEffectApplycationPolicy = EffectApplycationPolicy::DoNotApply;
	//无限效果的移除策略，只有无限效果才有
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EffectRemovePolicy InfinitEffectRemovePolicy = EffectRemovePolicy::RemoveOnEndOverlap;
};
