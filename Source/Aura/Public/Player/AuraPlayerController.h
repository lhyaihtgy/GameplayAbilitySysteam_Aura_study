// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

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
	TObjectPtr<IEnemyInterface> ThisActor;
	//Tick检测中这一帧率，鼠标下的actor类型
	TObjectPtr<IEnemyInterface> LastActor;
};
