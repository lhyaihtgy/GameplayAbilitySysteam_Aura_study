// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AuraAttributeSet.generated.h"


/**
 * GAS属性访问器宏定义（UE官方推荐写法）
 * @brief 批量生成GAS属性的Get/Set/Init方法，避免重复编写冗余代码
 * @param ClassName 所属AttributeSet类名（如UAuraAttributeSet）
 * @param PropertyName 属性名（如Health、Mana）
 * 宏展开后包含4类核心方法：
 * 1. GAMEPLAYATTRIBUTE_PROPERTY_GETTER：获取属性的FGameplayAttribute句柄（用于GAS效果绑定）
 * 2. GAMEPLAYATTRIBUTE_VALUE_GETTER：获取属性的当前数值（float类型）
 * 3. GAMEPLAYATTRIBUTE_VALUE_SETTER：设置属性的数值（需通过ASC操作，此处为基础Setter）
 * 4. GAMEPLAYATTRIBUTE_VALUE_INITTER：初始化属性数值（Actor生成时调用）
 * 注意：该宏仅生成方法声明，实现由UE GAS框架自动处理
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()
	FEffectProperties()
	{}
	UPROPERTY()
	FGameplayEffectContextHandle EffectContextHandle;
	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;
	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;
	UPROPERTY()
	AController* SourceController = nullptr;
	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;
	
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;
	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;
	UPROPERTY()
	AController* TargetController = nullptr;
	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};
/**
 * @brief 角色核心属性集（AttributeSet），基于UE GAS框架实现
 * @note 作用：统一管理角色的生命值、魔力值等核心属性，提供GAS标准的属性同步、修改、回调机制
 * 核心特性：
 * 1. 所有属性继承FGameplayAttributeData，适配GAS的效果修改逻辑；
 * 2. 支持网络同步，通过ReplicatedUsing回调实现客户端属性更新；
 * 3. 重写PreAttributeChange实现属性修改前的校验（如生命值不超过最大值）；
 */
UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	/**
	 * @brief 构造函数：初始化属性集的默认配置
	 * @note 执行时机：AttributeSet实例创建时（与持有该组件的Actor同步创建）
	 * 核心用途：初始化属性默认值、绑定属性修改回调（可选）
	 */
	UAuraAttributeSet();
	
	/**
	 * @brief 重写网络同步属性注册函数（GAS属性同步核心）
	 * @note 执行时机：AttributeSet初始化时，UE自动调用该函数注册需要网络同步的属性
	 * @param OutLifetimeProps 输出参数，需将需要同步的属性添加到该数组中
	 * 核心作用：标记哪些属性需要在服务端和客户端之间同步（如Health、MaxHealth）
	 */
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	/**
	 * @brief 重写属性修改前的校验函数
	 * @note 执行时机：任何GAS效果/代码修改属性数值前触发（服务端+客户端）
	 * @param Attribute 即将被修改的属性（如Health）
	 * @param NewValue 属性即将被设置的新值
	 * 核心用途：限制属性数值范围（如生命值不能超过最大生命值、不能小于0），避免非法数值
	 */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	/**
	 * @brief 角色当前生命值（GAS标准属性）
	 * @note 特性：
	 * 1. BlueprintReadOnly：蓝图中仅可读取，禁止直接修改（需通过ASC/GAS效果修改）；
	 * 2. ReplicatedUsing = OnRep_Health：网络同步，属性变化时触发OnRep_Health回调；
	 * 3. Category = "Vital Attributes"：蓝图编辑器中归类，便于管理；
	 * 4. 类型为FGameplayAttributeData：GAS框架标准属性类型，包含数值和修改记录；
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	// 通过宏生成该属性的Get/Set/Init方法（如GetHealth()、SetHealth()、InitHealth()）
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);
	
	/**
	 * @brief 角色最大生命值（GAS标准属性）
	 * @note 特性与Health一致，用于限制生命值的上限
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);
	
	/**
	 * @brief 角色当前魔力值（GAS标准属性）
	 * @note 特性与Health一致，用于角色技能释放消耗/恢复
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);
	
	/**
	 * @brief 角色最大魔力值（GAS标准属性）
	 * @note 特性与MaxHealth一致，用于限制魔力值的上限
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);

	/**
	 * @brief 生命值同步回调函数（客户端触发）
	 * @note 触发时机：服务端Health属性变化，同步到客户端时调用
	 * @param OldHealth 同步前的生命值旧值
	 * 核心用途：
	 * 1. 客户端UI刷新（如血条更新）；
	 * 2. 触发视觉/音效反馈（如受伤特效、加血音效）；
	 * 3. 客户端逻辑校验（如生命值为0时触发死亡逻辑）；
	 */
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	/**
	 * @brief 最大生命值同步回调函数（客户端触发）
	 * @note 触发时机：服务端MaxHealth属性变化，同步到客户端时调用
	 * @param OldMaxHealth 同步前的最大生命值旧值
	 * 核心用途：
	 * 1. 客户端UI刷新（如血条最大值更新）；
	 * 2. 重新计算生命值百分比（如最大生命值提升后，血条占比变化）；
	 */
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	
	/**
	 * @brief 魔力值同步回调函数（客户端触发）
	 * @note 触发时机：服务端Mana属性变化，同步到客户端时调用
	 * @param OldMana 同步前的魔力值旧值
	 * 核心用途：客户端蓝条UI刷新、技能可用状态更新
	 */
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;
	
	/**
	 * @brief 最大魔力值同步回调函数（客户端触发）
	 * @note 触发时机：服务端MaxMana属性变化，同步到客户端时调用
	 * @param OldMaxMana 同步前的最大魔力值旧值
	 * 核心用途：客户端蓝条最大值刷新、魔力值百分比重新计算
	 */
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;
private:
	void SetFEffectProperties(const FGameplayEffectModCallbackData& Data,FEffectProperties& Props) const;
};