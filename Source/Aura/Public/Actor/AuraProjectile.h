// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraProjectile.generated.h"

struct FGameplayEffectSpecHandle;
class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
/*
 *这个类的作用是所有弹射actor的父类，所有弹射actor都要继承这个类，这个类中包含了一个球形的碰撞体，所有弹射actor都要使用这个碰撞体来进行碰撞检测
 *
 */
UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AAuraProjectile();

	//下面这个成员变量是所有弹射actor的移动组件，所有弹射actor都要使用这个移动组件来进行移动，这个移动组件是解决弹射物物体移动的组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
	UPROPERTY(BlueprintReadWrite,meta=(ExposeOnSpawn=true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;
protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,const FHitResult& SweepResult);
private:
	
	//飞行物的存活时间
	UPROPERTY(EditDefaultsOnly)
	float Lifespan = 15.0f;
	//在销毁飞行物前客户端可能检测到了这个重叠事件客户端就将这个值设定为true代表这个特效已经处理过了，同时这个变量也能处理是否这个飞行物的销毁，在客户端检测之前。
	bool bHits = false;
	//这是所有弹射actor的碰撞体，一个球形的碰撞体
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
	
	//飞行物的撞击特效
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	//飞行物的撞机音效，使用USoundBase代表这个成员变量既可以接受元声音还可以接受音效提示
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;
	
	//飞行物体，飞行时间内的持续音效
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;
	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;
};
