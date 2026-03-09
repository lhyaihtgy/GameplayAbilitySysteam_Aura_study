// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


AAuraProjectile::AAuraProjectile()
{
 	//让这个类的实例每帧禁止调用tick函数，我也没有实现tick函数
	PrimaryActorTick.bCanEverTick = false;
	//下面这行代码的作用是让这个类的实例在网络游戏中进行复制，这样这个类的实例就可以在服务器和客户端之间进行同步了
	bReplicates = true;
	
	Sphere = CreateDefaultSubobject<USphereComponent>("sphere");
	SetRootComponent(Sphere);
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovementComponent->InitialSpeed = 550.0f;
	ProjectileMovementComponent->InitialSpeed = 550.0f;
	//下面是设定这个移动组件的重力缩放为0，这样这个移动组件就不会受到重力的影响了，这样这个移动组件就可以一直保持在空中飞行了
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
	//下面这行代码的作用是设置碰撞体的碰撞响应为只进行查询，这样就不会对其他物体造成物理影响了，只会触发重叠事件
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//下面这行代码的作用是设置碰撞体的碰撞响应为忽略所有通道，这样就不会对其他物体造成物理影响了，只会触发重叠事件
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	//下面这行代码的作用是设置碰撞体的碰撞响应为忽略世界动态通道，这样就不会对其他物体造成物理影响了，只会触发重叠事件
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Ignore);
	//下面这行代码的作用是设置碰撞体的碰撞响应为忽略世界静态通道，这样就不会对其他物体造成物理影响了，只会触发重叠事件
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Ignore);
	//下面这行代码的作用是设置碰撞体的碰撞响应为忽略角色通道，这样就不会对其他物体造成物理影响了，只会触发重叠事件
	Sphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore);
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}



