// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"



class UAuraUserWidget;
struct FOnAttributeChangeData;
USTRUCT(BlueprintType)
struct FUIWidgetRow:public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MesssageTag = FGameplayTag();
	
	
	//向小组件中传递的信息
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Messsage = FText();
	
	//对于任意的游戏玩法标签，这里传入一个小部件，让在接受到这个标签
	//对应的效果时，能够将其显示到玩家UI上
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};
/*
// 声明「血量变化」的动态多播委托（带1个浮点型参数）
// 动态多播委托：支持C++和蓝图双向绑定/触发，可同时绑定多个回调函数
// 参数说明：float NewHealth → 血量变化后的最新血量值
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature,float,NewHealth);
// 声明「最大血量变化」的动态多播委托（带1个浮点型参数）
// 参数说明：float NewMaxHealth → 最大血量变化后的最新上限值
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSignature,float,NewMaxHealth);
//和上面一致只不过是蓝量的
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaChangedSignature,float,NewMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxManaChangedSignature,float,NewMaxMana);
*/
//以上写法优化为声明一个动态多播委托即可，因为上面的委托就只是将一个新的float值传递出去
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature,float,NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature,FUIWidgetRow,ROW);



/**
 * Overlay UI（血条/蓝条面板）的逻辑控制器
 * 负责属性数据的监听、计算，并通过委托通知UI更新显示
 */
UCLASS(Blueprintable,BlueprintType)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	
	//广播初始值
	virtual void BroadcastInitialValues() override;
	
	virtual void BindCallbacksToDependencies() override;
	
	/**
	 * 【血量变化委托】- 蓝图可绑定
	 * 用途：当玩家血量发生变化时，触发该委托通知UI更新血条显示
	 * BlueprintAssignable：允许在蓝图中绑定该委托的事件（比如UI蓝图里绑定“更新血条进度”逻辑）
	 * Category = "GAS|Attributes"：在蓝图细节面板中归类，方便查找（GAS=能力系统，Attributes=属性）
	 */
	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	
	/**
	 * 【最大血量变化委托】- 蓝图可绑定
	 * 用途：当玩家最大血量上限变化时（比如升级、加buff），触发该委托通知UI更新血条上限显示
	 * BlueprintAssignable：支持蓝图中绑定回调（比如UI蓝图里调整血条总长度、显示最大血量数值）
	 * Category = "GAS|Attributes"：归类到GAS属性模块，和其他属性委托统一管理
	 */
	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
	//和上面一致只不过是蓝量的
	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;
	
	//和上面一致只不过是蓝量的
	UPROPERTY(BlueprintAssignable,Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;
	
	UPROPERTY(BlueprintAssignable,Category = "GAS|Message")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;
	
protected:
	
	
	//这个容器表中储存着一些信息，当gameEffect启效果的时候，会传入一个标签容器，查找这个容器内部的标签是否在这个表中存在，存在就将这个标签内部挂在的内容广播给小组件
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Weiget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;
	
	//这个模板函数之后会写入静态函数库中作用是接受任意类型的数据表并从中找到Tag对应的那一行
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable,const FGameplayTag& Tag);
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(),TEXT(""));
}
