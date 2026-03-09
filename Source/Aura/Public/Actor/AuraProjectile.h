// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
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
protected:
	virtual void BeginPlay() override;

	
	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,const FHitResult& SweepResult);
private:
	
	//这是所有弹射actor的碰撞体，一个球形的碰撞体
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

};
