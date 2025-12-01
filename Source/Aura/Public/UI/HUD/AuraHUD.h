// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
class UAuraUserWidget;
class UOverlayWidgetController;
struct FWidgetControllerParams;
/**
 * 
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()
public:
	
	/**
	 * 屏幕上实际显示的UI控件（比如血条、蓝条、技能栏这些看得见的UI）
	 * 简单说：这是程序运行后真正创建出来、能在游戏里看到的UI对象
	 * 用TObjectPtr是为了防止UI对象被销毁后，这个指针变成无效的“野指针”导致崩溃
	 */
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;
	
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	
	UOverlayWidgetController * GetOverlayWidgetController(const FWidgetControllerParams& PCparms);
	
	void InitOverlay(APlayerController* PC,APlayerState* PS,UAbilitySystemComponent* ASC,UAttributeSet* AS);

private:
	/**
	 * UI的“设计图纸”（在编辑器里可以直接选要用哪个UI蓝图）
	 * 简单说：程序运行时，会根据这个“图纸”创建出上面的OverlayWidget（实际显示的UI）
	 * EditAnywhere：不用改代码，在编辑器的属性面板里就能选对应的UI蓝图
	 * 私有化：不让外部代码随便改这个“图纸”，只让HUD自己控制用哪个UI
	 */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
};
