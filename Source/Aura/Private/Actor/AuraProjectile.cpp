// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"


AAuraProjectile::AAuraProjectile()
{
 	//让这个类的实例每帧禁止调用tick函数，我也没有实现tick函数
	PrimaryActorTick.bCanEverTick = false;
	//下面这行代码的作用是让这个类的实例在网络游戏中进行复制，这样这个类的实例就可以在服务器和客户端之间进行同步了
	bReplicates = true;
	
	Sphere = CreateDefaultSubobject<USphereComponent>("sphere");
	SetRootComponent(Sphere);
	//设定碰撞类型
	Sphere->SetCollisionObjectType(ECC_Projectile);
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovementComponent->InitialSpeed = 550.0f;
	ProjectileMovementComponent->InitialSpeed = 550.0f;
	//下面是设定这个移动组件的重力缩放为0，这样这个移动组件就不会受到重力的影响了，这样这个移动组件就可以一直保持在空中飞行了
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	//设定这个飞行物的生命周期
	SetLifeSpan(Lifespan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
	//下面这行代码的作用是设置碰撞体的碰撞响应为只进行查询，这样就不会对其他物体造成物理影响了，只会触发重叠事件
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//显式开启重叠事件，确保OnComponentBeginOverlap能够触发
	Sphere->SetGenerateOverlapEvents(true);
	//先忽略全部，再只对需要检测的通道开启重叠
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	//与动态物体重叠（如可移动Actor）
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Overlap);
	//与静态场景重叠（如墙体、地面）
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Overlap);
	//与角色胶囊体重叠（玩家/敌人）
	Sphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	
	//创建飞行物飞行过程中的循环音效，这个音效组件不属于任何类（我写的类中），我们可以保存下来，确保什么时候关闭循环音效
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound,GetRootComponent());
}

void AAuraProjectile::Destroyed()
{
	//如果客户端检测到这个飞行物没有被处理过，并且到这代表服务端已经准备销毁这个飞行物了，此时就让飞行物准备释放音效和粒子特效
	if (!bHits&&!HasAuthority())
	{
		//在飞行物撞击到物体的开始释放音效
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound,GetActorLocation(),FRotator::ZeroRotator);
		//释放撞击的粒子特效
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ImpactEffect,GetActorLocation());
		if (LoopingSoundComponent)
		{
			LoopingSoundComponent->Stop();
		}
	}
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//在飞行物撞击到物体的开始释放音效
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound,GetActorLocation(),FRotator::ZeroRotator);
	//释放撞击的粒子特效
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ImpactEffect,GetActorLocation());
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
	}

	//服务端销毁这个飞行物
	if (HasAuthority())
	{
		//如果飞行物击中的这个物体也有能力系统组件就将Damage所指代的这个游戏效果赋予给这个actor的能力系统组件，让其修改自己的属性
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
		Destroy();
	}
	else
	{
		//客户端设定这个飞行物体的特效已经处理过了
		bHits = true;
	}
}



