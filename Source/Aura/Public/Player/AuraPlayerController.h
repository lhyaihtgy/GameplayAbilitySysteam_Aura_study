// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraPlayerController.generated.h"

class USplineComponent;
class UAuraInputConfig;
struct FInputActionValue;
class UInputMappingContext;	
class UInputAction;
class IEnemyInterface;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;//配置输入组件，将输入动作（如移动）与对应的处理函数绑定，是输入系统初始化的关键步骤。 
private:
	UPROPERTY(EditAnywhere,Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	
	UPROPERTY(EditAnywhere,Category = "Input")
	TObjectPtr<UInputAction> MoveAction;//用来保存移动输入数据的变量
	
	void Move(const FInputActionValue& InputActionValue);//处理输入数据的函数
	
	//这个函数由玩家操作器每帧调用，检测鼠标下的actor是否重写了高亮接口，并且对高亮接口进行调用
	void CursorTrace();
	
	//Tick检测中上一帧，鼠标下的actor类型
	IEnemyInterface* ThisActor;
	//Tick检测中这一帧率，鼠标下的actor类型
	IEnemyInterface* LastActor;
	//储存鼠标光标检测的结果
	FHitResult CursorResults;
	
	//以下三个函数由增强输入组件调用，用于处理能力输入标签的按下、释放和持续按下事件，是按键按下时具体会执行的函数
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	UPROPERTY(EditDefaultsOnly,Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
 
	UAuraAbilitySystemComponent* GetAsc();
	
	//记录鼠标点击位置的向量坐标，也就是人物移动的终点坐标
	FVector CachedDestination = FVector::ZeroVector;
	//记录按下鼠标的时间
	float FollowTime = 0.0f;
	//短按的时间阈值，超过这个时间就是长按
	float ShortPressThreshold = 0.5f;
	//自动寻路是否开启，开启那么每一帧都要调用移动函数
	bool bAutoRuning = false;
	//因为鼠标左键点击比较特殊，当鼠标左键点击到了敌人身上，此时应该释放左边绑定的技能，而没有点击到敌人身上就应该进行移动
	//这个变量就是判断是否点击到了敌人身上
	bool bTargeting = false;
	//自动移动的时候距离目的地会越来越近，当距离目的地多远的时候会进行一个操作（这个操作可以是停止自动移动）
	//该变量定义多远
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.0f;
	
	//储存从初始地到目的地计算出来的曲线，方便移动使用
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;
	
	//鼠标短按后自动移动的处理逻辑
	void AutoRun();
};
