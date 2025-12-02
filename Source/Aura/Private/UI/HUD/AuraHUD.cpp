// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"


/**
 * 获取/创建Overlay UI对应的WidgetController（UI逻辑控制器）
 * 采用单例模式：仅当控制器为空时新建，避免重复创建，保证UI逻辑唯一
 * @param PCparms 传递给WidgetController的核心参数（玩家控制器、状态、能力系统等）
 * @return 初始化完成的OverlayWidgetController（UI逻辑控制器）
 */
UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& PCparms)
{
	// 判空：仅当控制器未创建时，才新建实例（单例逻辑，避免重复创建）
	if (OverlayWidgetController == nullptr)
	{
		// 新建OverlayWidgetController实例：
		// this：指定HUD为该对象的Outer（所有者），保证内存管理安全；
		// OverlayWidgetControllerClass：编辑器配置的蓝图模板，决定创建哪个子类实例
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this,OverlayWidgetControllerClass);
		// 给控制器设置核心参数（玩家控制器、状态等），让控制器能访问游戏核心数据
		OverlayWidgetController->SetWidgetControllerParams(PCparms);
		//再将基础属性值变化的时候的对应绑定函数进行绑定
		OverlayWidgetController->BindCallbacksToDependencies();
		return OverlayWidgetController;
	}
	// 控制器已存在，直接返回（避免重复初始化）
	return OverlayWidgetController;
}

/**
 * 初始化HUD核心Overlay UI（血条/蓝条/属性面板等）
 * 核心逻辑：创建UI控件 → 初始化UI逻辑控制器 → 绑定控制器到UI → 显示UI
 * @param PC 玩家控制器（提供输入/视角等上下文）
 * @param PS 玩家状态（存储玩家等级/血量等持久化数据）
 * @param ASC 能力系统组件（处理技能/属性计算）
 * @param AS 属性集（存储玩家具体属性值：血量、蓝量等）
 */
void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	// 强制检查：OverlayWidgetClass（UI蓝图模板）未配置时，触发断言并提示（防止运行时崩溃）
	// 需在BP_AuraHUD的细节面板中选择对应的Overlay UI蓝图
	checkf(OverlayWidgetClass,TEXT("Overlay Widget Class uninitialized,please fill out BP_AuraHUD"));
	
	// 强制检查：OverlayWidgetController（UI逻辑控制器模板）未配置时，触发断言并提示
	// 需在BP_AuraHUD的细节面板中选择对应的WidgetController蓝图
	checkf(OverlayWidgetControllerClass,TEXT("Overlay WidgetController Class uninitialized,Please fill out BP_AuraHUD"));
	


	// 根据UI蓝图模板创建Overlay UI实例（先创建父类UUserWidget，后续强转为自定义子类）
	UUserWidget*Widget = CreateWidget<UUserWidget>(GetWorld(),OverlayWidgetClass);
	
	// 将通用UUserWidget强转为自定义UAuraUserWidget，方便调用自定义方法（如SetWidgetController）
	OverlayWidget = Cast<UAuraUserWidget>(Widget);

	// 构建WidgetController所需的参数结构体：整合玩家核心数据，统一传递给控制器
	const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
	
	// 获取/创建Overlay对应的UI逻辑控制器（保证控制器唯一且参数初始化完成）
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);
	
	// 将UI逻辑控制器绑定到UI控件：实现“UI显示”和“逻辑处理”分离（MVVM设计思路）
	// 后续UI的数值更新、事件响应，都由WidgetController驱动，UI仅负责显示
	//这个函数调用的时候会调用一个WidgetControllerSet的函数，在那个函数中使用蓝图实现将一个大OverlayWidget中的两个小的Widget的WidgetController设置为了大OverlayWidget的WidgetController
	//因为这个函数就是设置大OverlayWidget的WidgetController，能够保证两个小Widget的Controller一定被成功创建
	OverlayWidget->SetWidgetController(WidgetController);
	
	// 二次确认参数设置（兜底：防止控制器参数未正确初始化）
	WidgetController->SetWidgetControllerParams(WidgetControllerParams);

	//当运行到这里的时候AttributeSet已经初始化完成了，同时OverlayWidget已经设定了自己的WidgetController，下面就是OverlayWidget的WidgetController进行广播初始化值了
	WidgetController->BroadcastInitialValues();
	
	
	// 将Overlay UI添加到游戏视口，玩家屏幕上可见该UI
	Widget->AddToViewport();
}


