// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

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
