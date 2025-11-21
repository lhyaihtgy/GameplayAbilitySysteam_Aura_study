// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* substeam = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	
	//通过增加玩家输入子系统来映射上下文，由此让玩家控制器获得数据
	check(substeam);
	substeam->AddMappingContext(AuraContext,0);
	
	
	bShowMouseCursor = true;//设定显示鼠标
	DefaultMouseCursor = EMouseCursor::Default;//鼠标样式为默认
	
	FInputModeGameAndUI InputModData;
	InputModData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModData);
	
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);//将默认输入组件替换为增强输入组件
	
	EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);//获得输入数据之后将由move函数来处理
	
}

//对于移动输入的具体逻辑在这个函数中进行实现
void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	//从输入动作值中获取二维向量，这里指的是WASD的组合(W和S，A和D)，范围通常为(-1,-1)到(1,1)
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	
	//获取玩家控制器的旋转信息
	const FRotator Rotation = GetControlRotation();
	//提取偏航角yaw，都读构成宣祖安，pitch和roll都设置为0，代表只保留左右转向的影响，之一步是为了让角色移动方向和玩家视角朝向对其，比如视角转向后，w对应的是朝视角前方移动
	const FRotator YawRotation(0.f,Rotation.Yaw,0.f);
	
	//通过旋转矩阵将偏航角转换为世界空间的方向向量
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);	
	
	//获取当前控制器操作的Pawn实体，if存在那么开始执行移动逻辑
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		//ForwardDirection 结合 InputAxisVector.Y（纵向输入，如 W/S 键、摇杆前后）：控制角色前后移动。
		//RightDirection 结合 InputAxisVector.X（横向输入，如 A/D 键、摇杆左右）：控制角色左右移动。
		//最终，Pawn 会根据这些输入向量的组合，在世界中产生移动。
		ControlledPawn->AddMovementInput(ForwardDirection,InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection,InputAxisVector.X);
	}
}
