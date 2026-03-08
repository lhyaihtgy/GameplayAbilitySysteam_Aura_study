// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	//实例化曲线变量
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}
void AAuraPlayerController::AutoRun()
{
	//如果不在自动移动状态就不执行自动移动的逻辑
	if (!bAutoRuning) return;
	//检测这个controller控制的是否是一个正确的角色
	if (APawn* Controlledpawn = GetPawn())
	{
		//获取这个角色到当前鼠标点击位置的路径上距离角色最近的点的位置坐标
		const FVector LocationSpline = Spline->FindLocationClosestToWorldLocation(Controlledpawn->GetActorLocation(),ESplineCoordinateSpace::World);
		//获取这个角色到当前鼠标点击位置的路径上距离角色最近的点的切线方向向量
		//Direction决定了在自动移动的时候我们的角色移动的方向
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationSpline,ESplineCoordinateSpace::World);
		Controlledpawn->AddMovementInput(Direction);
		//获取当前角色位置到鼠标点击位置的距离
		const float DistanceToDestination = (LocationSpline-CachedDestination).Length();
		if (DistanceToDestination<AutoRunAcceptanceRadius)
		{
			//到达鼠标点击的附近，就停止自动移动
			bAutoRuning = false;
		}
		
	}
}
void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	//每帧检测是否需要高亮当前鼠标下的actor
	CursorTrace();
	//自动移动的处理函数，每帧调用
	AutoRun();
}
void AAuraPlayerController::CursorTrace()
{
	/*
	 * 精简版本
	 */
	GetHitResultUnderCursor(ECC_Visibility, false, CursorResults);
	if (!CursorResults.bBlockingHit) return;
	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorResults.GetActor());
	if (ThisActor!=LastActor)
	{
		if (LastActor)
		{
			//这一帧鼠标下的敌人actor，不是上一帧鼠标下的敌人actor，并且上一帧鼠标下的敌人actor是有效的，那么就取消上一帧鼠标下的敌人actor的高亮
			LastActor->UnHightLightEnemy();
		}
		//这一帧鼠标下的敌人actor，不是上一帧鼠标下的敌人actor，并且这一帧鼠标下的敌人actor是有效的，那么就高亮这一帧鼠标下的敌人actor
		if (ThisActor) ThisActor->HightLightEnemy();
	}
	
	
	
	/*下面是详细带注释但是不精简版本
	//需要检测当前鼠标下的actor
	
	
	
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
	/*
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
	}*/
	
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//当前标签对应的就是鼠标左键的标签，代表鼠标左键被按下了
	if (InputTag == FAuraGameplayTags::Get().InputTag_LMB)
	{
		//当上一帧中判断目标点击物体是一个actor，并且实现了高亮代表是敌人，此时敌人标签就是true，点击左键，应该释放技能，反之移动
		bTargeting = ThisActor? true: false;
		//刚刚按下左键时，并不知道当前这个点击时长还是短此时不能启动自动移动
		bAutoRuning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	//如果按下的并不是鼠标左键，那就要激活对应按键标签对应的技能
	if (!(InputTag == FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetAsc())
			GetAsc()->AbilityInputTagReleased(InputTag);
		return;
	}
	//按下鼠标左键，并且鼠标左键对应的是敌人需要激活鼠标左键对应的技能
	if (bTargeting)
	{
		if (GetAsc())
			GetAsc()->AbilityInputTagHeld(InputTag);
	}
	else
	{
		const APawn* ControlledPawn = GetPawn();
		//鼠标左键短按到一个地点了，我要进行短按的逻辑
		//首先判断时间是否符合短按的条件
		if (FollowTime<=ShortPressThreshold&&ControlledPawn)
		{
			//需要创建一个导航路径点数组
			if (UNavigationPath* Navpath =  UNavigationSystemV1::FindPathToLocationSynchronously(this,ControlledPawn->GetActorLocation(),CachedDestination))
			{
				//Navpath中就是一个导航路径，PathPoints就是这个导航路径中的路径点数组,玩家可能点击到一个永远无法到达的位置
				//所以需要将导航数组上的最后一个点更新为目标点
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc:Navpath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc,ESplineCoordinateSpace::World);
			
				}
				CachedDestination = Navpath->PathPoints[Navpath->PathPoints.Num()-1];
				//短按左键且无锁定敌人时自动寻路
				bAutoRuning = true;
			}
		}
		//这是松开左键的函数
		FollowTime = 0;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	//如果长按下的并不是鼠标左键，那就要激活对应按键标签对应的技能
	if (!(InputTag == FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetAsc())
			GetAsc()->AbilityInputTagHeld(InputTag);
		return;
	}
	//长按鼠标左键，并且鼠标左键对应的是敌人需要激活鼠标左键对应的技能
	if (bTargeting)
	{
		if (GetAsc())
			GetAsc()->AbilityInputTagHeld(InputTag);
	}
	else
	{
		//这里就是处理长按点击移动的逻辑
		
		//将按下左键的每一帧的时间机器加到左键按下时间记录的变量里面
		FollowTime += GetWorld()->GetDeltaSeconds();
		
		//从cursorResults中获取当前鼠标点击位置的碰撞结果
		if (CursorResults.bBlockingHit)
		{
			//这个点击的位置就是目标位置
			CachedDestination = CursorResults.ImpactPoint;
		}
		
		//确保这是受控制的角色
		if (APawn* ControlledPawn = GetPawn())
		{
			//目标位置的向量-受控制的角色位置的向量就是从受控制的角色位置到目标位置的路径
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			//调用被控制角色的移动组件移动角色到目标地点
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAsc()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
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
	//将默认输入组件替换为Aura自定义的增强输入组件
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	
	//获得输入数据之后将由move函数来处理
	AuraInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);
	//绑定能力输入标签的按下、释放和持续按下事件
	AuraInputComponent->BindAbilityAction(InputConfig,this,&ThisClass::AbilityInputTagPressed,&ThisClass::AbilityInputTagReleased,&ThisClass::AbilityInputTagHeld); 
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


