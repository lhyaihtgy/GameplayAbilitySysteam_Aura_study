// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/SphereComponent.h"

/**
 * @brief 构造函数：初始化Actor的核心组件与基础配置
 * @note 执行时机：Actor实例创建时（早于BeginPlay），仅初始化组件对象，不涉及场景交互逻辑
 * 核心逻辑：创建静态网格组件（Mesh）和球形碰撞组件（Sphere），并建立组件层级关系
 */
AAuraEffectActor::AAuraEffectActor()
{
 	// 禁用Actor的Tick更新（该Actor仅响应碰撞事件，无需帧更新，节省性能）
	PrimaryActorTick.bCanEverTick = false;
	
	// 创建静态网格组件（用于显示特效Actor的视觉模型），命名为"StaticMesh"
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	// 将Mesh设为根组件，作为Actor的空间定位基准
	SetRootComponent(Mesh);
	//将静态的mesh作为根
	
	// 创建球形碰撞组件（用于检测范围重叠），命名为"Sphere"
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	// 将Sphere组件附着到根组件（Mesh）上，继承根组件的Transform（位置/旋转/缩放）
	Sphere->SetupAttachment(GetRootComponent());
}

/**
 * @brief 碰撞重叠触发的回调函数
 * @param OverlappedComponent 发生重叠的当前Actor的Primitive组件（本示例中为Sphere碰撞体）
 * @param OtherActor 与当前Actor发生重叠的其他Actor（如玩家角色、敌人等）
 * @param OtherComp 其他Actor上参与重叠的Primitive组件
 * @param OtherBody 其他Actor的碰撞体索引（多碰撞体时区分具体碰撞部位）
 * @param bFromSweep 是否由Sweep扫描（如移动检测）触发的重叠，而非静态重叠
 * @param SweepResult 扫描触发重叠时的碰撞结果（包含位置、法线等信息）
 * @note 该函数需绑定到碰撞组件的OnComponentBeginOverlap委托才会触发
 */
//DOTO:之后要通过Game Effect来改变属性集中的属性，这里的去除cost是不和法的
void AAuraEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBody, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		//从我重写的接口中得到能力系统组件再去获得属性set，最后返回的是const的属性set，因为一般只有属性set自己来设置自己的属性值，而不能向下面这样通过cost_cast去除const属性之后再去修改值
		//这是不合法的
		const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UAuraAttributeSet::StaticClass()));
		
		//去除AuraAttributeSet的const属性，工作中绝对不能做，这里只是为了学习，
		UAuraAttributeSet* MutableAuraAttributeset = const_cast<UAuraAttributeSet*>(AuraAttributeSet);
		
		MutableAuraAttributeset->SetHealth(MutableAuraAttributeset->GetHealth()+25.0f);
		MutableAuraAttributeset->SetMana(MutableAuraAttributeset->GetMana()-25.0f);
		Destroy();
	}
}

/**
 * @brief 碰撞重叠结束触发的回调函数
 * @param OverlappedComponent 结束重叠的当前Actor的Primitive组件（本示例中为Sphere碰撞体）
 * @param OtherActor 与当前Actor结束重叠的其他Actor
 * @param OtherComp 其他Actor上参与重叠的Primitive组件
 * @param OtherBodyIndex 其他Actor的碰撞体索引（多碰撞体时区分具体碰撞部位）
 * @note 该函数需绑定到碰撞组件的OnComponentEndOverlap委托才会触发
 */
void AAuraEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

/**
 * @brief Actor初始化完成后的核心入口函数
 * @note 执行时机：Actor被生成且所有组件初始化完成后触发，晚于构造函数
 * 核心逻辑：初始化碰撞重叠的委托绑定，确保Sphere碰撞体检测到重叠时能触发OnOverlap回调
 */
void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	// 绑定碰撞开始重叠委托：当Sphere组件检测到其他对象进入范围时，触发OnOverlap函数
	// AddDynamic：UE特有的动态委托绑定宏，关联"委托源(Sphere的重叠事件)"与"回调函数(OnOverlap)"
	Sphere->OnComponentBeginOverlap.AddDynamic(this,&AAuraEffectActor::OnOverlap);
	// 绑定碰撞结束重叠委托：当Sphere组件检测到其他对象离开范围时，触发EndOverlap函数
	Sphere->OnComponentEndOverlap.AddDynamic(this,&AAuraEffectActor::EndOverlap);
}



