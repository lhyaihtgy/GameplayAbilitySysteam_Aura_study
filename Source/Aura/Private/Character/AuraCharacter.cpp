// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"

/**
 * @brief 初始化玩家角色的GAS（Gameplay Ability System）核心信息
 * 核心作用：将PlayerState中承载的能力系统组件（ASC）与角色上下文绑定，
 * 并获取ASC和属性集（AttributeSet）的本地引用，为后续技能、属性操作提供基础
 */
void AAuraCharacter::InitAbilitySystemInfo()
{
    // 从角色获取对应的Aura自定义PlayerState（存储全局玩家状态，包含ASC和属性集）
    // GetPlayerState<T> 是模板函数，自动类型转换，确保获取到正确的PlayerState子类
    AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    // 断言检查：确保PlayerState存在（若为空则触发崩溃，提示开发者排查PlayerState创建问题）
    check(AuraPlayerState);
    
    // 初始化ASC的ActorInfo：绑定ASC的所属者（Owner）和化身角色（Avatar）
    // - 所属者（AuraPlayerState）：ASC的实际拥有者，PlayerState跨角色切换时数据不丢失
    // - 化身角色（this，当前AuraCharacter）：ASC实际作用的角色，技能、属性效果应用于该角色
    // 核心逻辑：玩家角色的技能系统由PlayerState承载（全局持久），但作用于当前控制的角色
    AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
    
    // 缓存PlayerState中的ASC到角色本地成员变量
    // 后续角色逻辑（如技能触发、属性查询）可直接通过本地引用访问，无需重复获取PlayerState
    AbilitySysteamComponent = AuraPlayerState->GetAbilitySystemComponent();
    // 缓存PlayerState中的属性集（AttributeSet）到角色本地成员变量
    // 属性集存储角色核心属性（血量、蓝量、攻击力等），本地缓存方便快速访问
    AttributeSet = AuraPlayerState->GetAttributeSet();
	
	// 判空玩家控制器（适配多人游戏架构核心逻辑）：
	// 多人游戏中服务器持有所有玩家的PlayerController，而客户端仅持有自身的PlayerController；
	// 若不判空，当客户端代码尝试获取其他玩家的PlayerController时会返回空指针，
	// 直接调用后续GetHUD()/InitOverlay()会导致客户端崩溃，此判断规避该场景的致命错误
	//if (AAuraPlayerController* AuraPlayerController = GetController<AAuraPlayerController>())//错误代码
	if (AAuraPlayerController*AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			//只有本地的玩家才具有AuraHUD服务器是不需要HUD的
			//当函数执行到这里的时候可以确定HUD需要的元素都已经初始化完毕了
			AuraHUD->InitOverlay(AuraPlayerController,AuraPlayerState,AbilitySysteamComponent,AttributeSet);	
		}
	}
	
	
	
}

/**
 * @brief 服务器端：控制器（Controller）附身角色时触发的回调
 * 触发时机：服务器上PlayerController/AIController与角色完成绑定（Possess）时
 * 核心作用：在服务器端初始化GAS信息，确保服务器拥有权威的技能系统上下文
 */
void AAuraCharacter::PossessedBy(AController* NewController)
{
    // 调用父类实现：完成控制器绑定的基础逻辑（如设置角色的Controller指针）
    Super::PossessedBy(NewController);
    
    // 服务器端初始化GAS信息
    // 关键前提：控制器已绑定，PlayerState已同步到角色，此时初始化ASC可确保ActorInfo完整（包含Controller、PlayerState、Avatar）
    InitAbilitySystemInfo();
}

/**
 * @brief 客户端：PlayerState同步到本地时触发的回调
 * 触发时机：客户端首次接收服务器同步的PlayerState，或PlayerState数据更新时
 * 核心作用：在客户端初始化GAS信息，确保客户端拥有与服务器一致的技能系统上下文
 */
void AAuraCharacter::OnRep_PlayerState()
{
    // 调用父类实现：完成PlayerState同步的基础逻辑（如更新角色的PlayerState指针）
    Super::OnRep_PlayerState();
    
    // 客户端初始化GAS信息
    // 关键前提：PlayerState已同步到客户端，此时可获取其中的ASC和属性集，完成ActorInfo绑定
    // 客户端初始化后，可响应服务器同步的技能效果、属性变更，播放本地视觉/音效反馈
    InitAbilitySystemInfo();
}

AAuraCharacter::AAuraCharacter()
{
	
	//启动移动方向驱动角色旋转
	//当该属性为true时，角色的旋转会自动对齐其移动方向（比如向前移动时角色面朝前方，斜向移动时角色朝斜向转动）。
	//常用于第三人称游戏中，让角色的朝向与移动方向保持一致，增强操作的直观性。
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	/*设置角色的旋转速度。
	FRotator(InPitch: 0.f, InYaw: 400.f, InRoll: 0.f)表示：
	俯仰角（Pitch，上下抬头）旋转速度为0（不允许上下旋转）；
	偏航角（Yaw，左右转向）旋转速度为400°/秒（数值越大，转向越灵敏）；
	翻滚角（Roll，侧翻）旋转速度为0（不允许侧翻）。
	该属性与bOrientRotationToMovement配合，定义角色转向时的 “加速度”，让转向过程更平滑或更灵敏。
	*/
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	
	/*作用：启用 **“平面约束”**，限制角色只能在某个平面（通常是 XY 平面，即地面）上移动。
	常用于地面类角色（如人类、步兵），防止角色在 Z 轴（垂直方向）上无意义的移动（除非主动跳跃）。*/
	GetCharacterMovement()->bConstrainToPlane = true;
	
	/*角色生成时 **“强制对齐到约束平面”**。
	与bConstrainToPlane配合，确保角色在创建时直接落在地面上，避免因初始位置偏差导致悬浮或穿墙。*/
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	
	// 禁用“控制器俯仰角（上下抬头/低头）”对角色俯仰角的驱动
	// 设为false后，角色的上下朝向不再跟随玩家视角的上下转动（通常由动画蓝图或移动逻辑独立控制）
	bUseControllerRotationPitch = false;

	// 禁用“控制器翻滚角（左右侧翻）”对角色翻滚角的驱动
	// 设为false后，角色不会因控制器的侧翻操作而发生姿态翻滚，保持角色直立姿态稳定（绝大多数游戏无需角色侧翻）
	bUseControllerRotationRoll = false;

	// 禁用“控制器偏航角（左右转向）”对角色偏航角的驱动
	// 设为false后，角色的左右朝向不再直接跟随玩家视角转动，通常配合“bOrientRotationToMovement = true”
	// 让角色转向由移动方向自动驱动（如向前移动时角色面朝前方，斜向移动时自动转向移动方向），适用于第三人称移动逻辑
	bUseControllerRotationYaw = false;
	/*以上的代码核心目的是打造一个移动方向驱动转向，仅在地面平面移动，转向灵敏且姿态稳定的角色移动参数配置*/
	
}

