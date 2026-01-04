// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySysteamComponent;
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraCharacterBase::InitAbilitySystemInfo()
{
}

//这个GameEffect的作用就是初始化角色自身，此时的source和target都是角色自身
void AAuraCharacterBase::ApplyEffectToself(TSubclassOf<UGameplayEffect> GameplayEffect, float level) const
{
	// 检查AbilitySystemComponent是否有效，防止空指针错误
	check(IsValid(GetAbilitySystemComponent()));
	// 检查传入的GameplayEffect类是否有效
	check(GameplayEffect);
	// 创建一个GameplayEffectContextHandle，用于传递关于游戏效果应用环境的信息（如施法者等）
	FGameplayEffectContextHandle ContextHandle =  GetAbilitySystemComponent()->MakeEffectContext();
	// 将自身（this）对象设置为效果的来源对象
	ContextHandle.AddSourceObject(this);
	// 基于指定的GameEffect类、等级和Context生成一个GameEffect规格描述（SpecHandle）
	const FGameplayEffectSpecHandle EffectSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffect,level,ContextHandle);
	// 使用生成的GameEffect规格将游戏效果应用到自己身上
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToself(DefaultPrimaryAttributes,1.f);
	ApplyEffectToself(DefaultSecondaryAttributes,1.f);
	//因为游戏一开始的时候初始生命值和魔力值都是等于最大生命值和魔力值的，所以需要让最大生命值和最大魔力值初始化完成之后
	//而最大生命值和最大魔力值又需要依赖一级属性所以需要先初始化一级属性
	//之后才能够初始化生命值和魔力值
	ApplyEffectToself(DefaultVitalAttributes,1.f);
}








