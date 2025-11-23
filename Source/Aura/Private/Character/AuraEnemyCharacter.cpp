// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraEnemyCharacter.h"

#include "Aura/Aura.h"
AAuraEnemyCharacter::AAuraEnemyCharacter()
{
	// 设置角色网格体（Mesh）对“可见性碰撞通道（ECC_Visibility）”的碰撞响应为“阻挡（ECR_Block）”
	// 核心作用：让该网格体在“可见性通道”的碰撞检测中被判定为“阻挡物”
	// 常见场景：配合鼠标选中检测（如GetHitResultUnderCursor），确保鼠标能“命中”该角色/物体
	// 补充说明：
	// - ECC_Visibility（可见性通道）：UE预设碰撞通道，专门用于UI交互、物体选中、视线检测等场景
	// - ECR_Block（阻挡响应）：碰撞时会阻挡该通道的检测射线/碰撞体，使检测函数能识别到该物体
	// 若需取消选中检测，可改为ECR_Ignore（忽略）；若需仅检测不阻挡，可改为ECR_Overlap（重叠）
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
}
void AAuraEnemyCharacter::HightLightEnemy()
{
	
	// 启用角色网格体的“自定义深度渲染”功能
	// 开启后，该网格体的深度信息会被写入独立的自定义深度缓冲区，用于后续后期处理（如描边、高亮）
	GetMesh()->SetRenderCustomDepth(true);

	// 设置角色网格体的“自定义深度模板值”为红色通道标识
	// 模板值用于后期处理时“精准识别该物体”，配合后处理材质（如仅对模板值为CUSTOM_DEPTH_RED的物体应用高亮）
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

	// 启用武器的“自定义深度渲染”功能
	// 使武器的深度信息也被纳入自定义深度缓冲区，支持和角色统一的后期处理逻辑
	Weapon->SetRenderCustomDepth(true);
	
	// 设置武器的“自定义深度模板值”为红色通道标识
	// 让武器和角色使用相同的模板值，确保后期处理时（如高亮）能同时作用于角色和武器
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	
}

void AAuraEnemyCharacter::UnHightLightEnemy()
{
	
	//关闭角色网格体的“自定义深度渲染”功能
	GetMesh()->SetRenderCustomDepth(false);
	
	//关闭武器的“自定义深度渲染”功能
	Weapon->SetCustomDepthStencilValue(false);
}
