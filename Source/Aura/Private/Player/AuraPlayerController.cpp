// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	//没帧检测是否需要高亮当前鼠标下的actor
	CursorTrace();
}
void AAuraPlayerController::CursorTrace()
{
	//需要检测当前鼠标下的actor
	
	//储存这一次检测的结果
	FHitResult CursorResults;
	
	// 检测鼠标光标正下方的碰撞对象，获取碰撞结果
	// 参数1：ECC_Visibility - 碰撞通道类型为"可见性通道"（仅检测设置了"可见性"碰撞响应的对象，常用于UI交互、选中检测等场景）
	// 参数2：false - 是否忽略复杂碰撞体（false表示不忽略，会检测复杂网格体的精确碰撞；true则只检测简化碰撞体，性能更高）
	// 参数3：CursorResults - 输出参数（FHitResult或TArray<FHitResult>类型），用于存储碰撞检测到的结果（如命中的Actor、碰撞位置、法线等信息）
	GetHitResultUnderCursor(ECC_Visibility, false, CursorResults);
	
	if (!CursorResults.GetActor()) return;
	
	//当这个函数调用的时候当前thisActor中的就是上一帧的actor
	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorResults.GetActor());
	/*
	 * 这次的射线检测有以下几个结果
	 *1. last和this都为空，代表玩家前后两帧鼠标下都没有任何敌方actor
	 *		do nothing
	 *2. last为空this不为空，代表玩家前一帧鼠标下没有actor，这一帧下出现了这个敌方actor，这个敌方actor第一次出现需要进行高亮
	 *		do HightLiaght()
	 *3. last不为空this为空，代表玩家前一帧鼠标下有敌方actor，这一帧下没有敌方actor，需要对上一帧下的敌方actor进行高亮取消
	 *		do UnHightLight
	 *4. 两个都不为空，但是last!=this，代表玩家前一帧下的敌方actor和这一帧下的敌方actor不同，需要对前一帧取消高亮，这一帧进行高亮
	 *		last do UnHightLight this do HightLight
	 *5.两个都不为空，但是last == this 代表玩家两帧指向的是同一个敌方actor，这个敌方actor在其他逻辑已经高亮了，这里不需要高亮
	 *		do nothing
	 */
	
	if (LastActor == nullptr)
	{
		if (ThisActor != LastActor)
		{
			//情况2
			ThisActor->HightLightEnemy();
		}
		else
		{
			//情况1
		}
	}
	else //上一帧的actor是有效的
	{
		if (ThisActor == nullptr)
		{
			//情况3
			LastActor->UnHightLightEnemy();
		}
		else
		{
			//两者都是有效的
			if (ThisActor != LastActor)
			{
				//情况4
				LastActor->UnHightLightEnemy();
				ThisActor->HightLightEnemy();
			}
			else
			{
				//情况5
			}
		}
	}
	
}
void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
	check(AuraContext);

	// 获取增强输入本地玩家子系统（该子系统是客户端LocalPlayer专属模块，服务器无LocalPlayer）
	// LocalPlayer仅存在于客户端，服务器完全没有，因此服务器调用GetSubsystem会返回空
	UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	
	// 判空原因（核心适配多人游戏+客户端异常场景）：
	// 1. 服务器执行此代码时：无LocalPlayer → subsystem为空，直接调用AddMappingContext会导致服务器崩溃（联机房间掉线）；
	// 2. 客户端场景：PlayerController的BeginPlay可能早于LocalPlayer初始化，或Enhanced Input插件未启用 → subsystem为空；
	// 3. 避免空指针调用AddMappingContext导致客户端闪退（尤其主机/移动端更敏感）
	if (subsystem)
	{
		// 为本地玩家添加输入映射上下文，让玩家控制器获得输入绑定数据（仅客户端有效）
		subsystem->AddMappingContext(AuraContext, 0);
	}
	
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


