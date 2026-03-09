// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"


void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	const bool BIsServer = HasAuthority(&ActivationInfo);
	if (!BIsServer) return;
	//GetAvatarActorFromActorInfo函数的作用是从ActorInfo中获取技能的AvatarActor，也就是技能的施法者，这个函数返回一个AActor类型的指针，需要将其转换为实现了ICombatInterface接口的指针，这样就可以调用ICombatInterface接口中的函数了
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
		FTransform SpawnTransform;
		//设置了投射物的位置
		SpawnTransform.SetLocation(SocketLocation);
		//还需要设置投射物的旋转，这样投射物就会朝着角色面向的方向飞行了
		AAuraProjectile* Projectile =  GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		//在这里需要给这个投射物法术增加一个GameplayEffectSpec用于给敌人造成伤害
		
		//下面的这行代码执行后就会生成一个弹射物产品，但是这个弹射物还没有被完全生成出来，
		//还没有被放置到世界中，还没有被激活，所以我们需要调用FinishSpawning函数来完成这个弹射物的生成过程，这样这个弹射物就会被放置到世界中，并且会被激活了
		Projectile->FinishSpawning(SpawnTransform);
	}
	
}
