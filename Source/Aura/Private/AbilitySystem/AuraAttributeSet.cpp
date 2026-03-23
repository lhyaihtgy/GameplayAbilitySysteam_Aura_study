// Fill out your copyright notice in the Description page of Project Settings.

/*
*在这里说明一下GE对产生效果的步骤：
*1. 所有生效GE的修饰器叠加计算 → 得到「待修改的原始NewValue」（比如100+50-1=149）
*2. 进入PreAttributeChange → 对「待修改的NewValue」做夹值（你原来的代码：149→100）
*3. 应用修改 → 把夹值后的NewValue赋值给实际属性（Health=100）
*4. 进入PostGameplayEffectExecute → 对「已修改后的实际属性值」做最终校验
 */
#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UAuraAttributeSet::UAuraAttributeSet()
{
    //这个函数就是通过ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);宏生成的
    //InitHealth(5.0f);
    //InitMana(5.0f);
    const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
    
    //版本1创建力量属性委托，将委托绑定静态函数，这个委托绑定的就是属性访问器，通过这个属性访问器函数能够得到属性的值，放入到map中
    //FAttributeSignature StrengthSignature;
    //StrengthSignature.BindStatic(GetStrengthAttribute);
    //TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, StrengthSignature);
    
    //智力属性
    //FAttributeSignature IntelligenceSignature;
    //IntelligenceSignature.BindStatic(GetIntelligenceAttribute);
    //TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, IntelligenceSignature);
    
    //韧性属性
    //FAttributeSignature ResilienceSignature;
    //ResilienceSignature.BindStatic(GetResilienceAttribute);
    //TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, ResilienceSignature);
    
    //活力属性
    //FAttributeSignature VigorSignature;
    //VigorSignature.BindStatic(GetVigorAttribute);
    //TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, VigorSignature);
    
    //版本2，直接储存这个静态函数
    //一级属性
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength,GetStrengthAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence,GetIntelligenceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience,GetResilienceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor,GetVigorAttribute);
    
    //二级属性
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor,GetArmorAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration,GetArmorPenetrationAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance,GetBlockChanceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance,GetCriticalHitChanceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage,GetCriticalHitDamageAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance,GetCriticalHitResistanceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration,GetHealthRegenerationAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegeneration,GetManaRegenerationAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth,GetMaxHealthAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMana,GetMaxManaAttribute);
}

/**
 * @brief 重写GAS属性集的生命周期复制属性注册函数
 * 核心作用：向UE网络系统注册需要跨服务器-客户端同步的属性，定义属性的同步规则（条件、是否强制触发回调）
 * 所有标记为Replicated的GAS属性，必须在此函数中通过DOREPLIFETIME系列宏注册，否则无法同步
 * @param OutLifetimeProps 输出参数，存储当前属性集所有需要同步的属性信息（由UE内部处理后续同步逻辑）
 */
void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    // 调用父类实现，确保父类中声明的可同步属性也能被注册（如GAS内置基础属性，避免遗漏）
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 注册Health属性的同步规则：
    // 1. DOREPLIFETIME_CONDITION_NOTIFY：带同步条件和回调触发策略的注册宏
    // 2. 模板参数：当前属性集类（UAuraAttributeSet）、要同步的属性（Health）
    // 3. COND_None：同步条件为"无限制"（只要属性变更，服务器就会同步到客户端，不额外过滤）
    // 4. REPNOTIFY_Always：强制触发OnRep_Health回调（无论同步的新值与客户端旧值是否一致，均执行回调）
    // 用途：确保客户端能实时响应生命值变更（如UI刷新、受伤特效），即使值未变也需触发（如特殊逻辑判定）
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);


    
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana,COND_None, REPNOTIFY_Always);
    
    
    //Primary Attributes
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength,COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence,COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience,COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor,COND_None, REPNOTIFY_Always);
    
    //Secondary Attributes
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor,COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration,COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance,COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance,COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage,COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance,COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration,COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration,COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana,COND_None, REPNOTIFY_Always);
    
    // 注册MaxHealth属性的同步规则（与Health逻辑一致）：
    // 同步条件无限制，强制触发OnRep_MaxHealth回调
    // 用途：最大生命值变更时（如升级、装备加成），客户端需及时刷新UI上限、属性面板等
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth,COND_None, REPNOTIFY_Always);
}

/**
 * @brief 属性修改前的数值校验函数（GAS核心回调）
 * @note 执行时机：任何GAS效果/代码修改属性数值前触发（服务端+客户端）
 * @param Attribute 即将被修改的目标属性（如Health、Mana）
 * @param NewValue 待设置的属性新值（按引用传递，可直接修改限制范围）
 * 核心逻辑：
 * 1. 限制生命值（Health）范围：0 ≤ 生命值 ≤ 最大生命值（MaxHealth）；
 * 2. 限制魔力值（Mana）范围：0 ≤ 魔力值 ≤ 最大魔力值（MaxMana）；
 * 作用：避免属性出现非法数值（如负数生命值、超过上限的魔力值）
 */
void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    // 调用父类的PreAttributeChange，确保GAS默认逻辑正常执行
    Super::PreAttributeChange(Attribute, NewValue);
    
    // 校验并限制生命值范围：不能小于0，也不能超过当前最大生命值
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
    // 校验并限制魔力值范围：不能小于0，也不能超过当前最大魔力值
    if (Attribute == GetManaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
    }
}

/**
 * @brief 解析GAS效果的源/目标属性并封装到FEffectProperties结构体
 * @note 核心工具函数：统一提取效果相关的核心对象，避免重复代码，提升可读性
 * @param Data GAS效果修改回调数据（包含效果规格、源/目标ASC等核心信息）
 * @param Props 输出参数：封装后的效果属性结构体（源/目标的ASC、Actor、Controller、Character等）
 * 关键定义：
 * - Source：触发效果的发起方（如释放技能的玩家）；
 * - Target：效果作用的目标方（当前AttributeSet的拥有者，如被攻击的敌人）；
 * 核心逻辑：
 * 1. 从效果上下文提取源端（Source）的ASC、Actor、Controller、Character；
 * 2. 从回调数据提取目标端（Target）的ASC、Actor、Controller、Character；
 * 3. 处理边界情况（如SourceController为空时，从Pawn补全）；
 */
void UAuraAttributeSet::SetFEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
    // 1. 从效果规格中获取效果上下文句柄（记录效果的发起者、来源等元数据）
    Props.EffectContextHandle = Data.EffectSpec.GetContext();
    
    // 2. 获取源端（Source）的AbilitySystemComponent（ASC）——效果发起方的核心GAS组件
    Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
    
    // 3. 解析源端（Source）的核心对象（ASC→AvatarActor→Controller→Character）
    if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
    {
        // 源端的AvatarActor（通常是发起效果的角色Actor，如玩家角色、敌人AI）
        Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
        // 源端的PlayerController（若存在，如玩家操控的角色；AI角色可能为空）
        Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
        
        // 边界处理：若SourceController为空（如AI角色），从SourceAvatarActor（Pawn）补全Controller
        if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
        {
            if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
            {
                Props.SourceController = Pawn->GetController();
            }
        }
        
        // 源端的Character（若SourceAvatarActor是Character类型，如玩家/敌人角色）
        if (Props.SourceController)
        {
           Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
        }
    }
    
    // 4. 解析目标端（Target）的核心对象（当前AttributeSet的拥有者）
    if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
    {
        // 目标端的AvatarActor（效果作用的角色Actor，如被加血/扣血的角色）
        Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
        // 目标端的PlayerController（若存在）
        Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
        // 目标端的Character（若TargetAvatarActor是Character类型）
        Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
        // 目标端的ASC（通过蓝图库获取，确保与TargetAvatarActor绑定）
        Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
    }
}

/**
 * @brief GAS效果执行后的回调函数（属性修改完成后触发）
 * @note 执行时机：任何GAS效果修改当前AttributeSet的属性后立即触发（如加血、扣蓝、加攻击后）
 * @param Data GAS效果修改回调数据（包含效果规格、源/目标信息、属性修改记录等）
 * 核心逻辑：
 * 1. 初始化效果属性结构体Props；
 * 2. 调用SetFEffectProperties解析源/目标的核心对象；
 * 3. 为后续业务逻辑（如血条UI刷新、受伤特效、死亡判断）提供基础数据；
 * 关键注意点：
 * - 效果发起方（Source）可能不包含Controller（如环境陷阱触发的效果）；
 * - Props中的属性可能部分无效（如SourceCharacter为空），使用前需通过IsValid()校验；
 */
void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    // 调用父类的PostGameplayEffectExecute，确保GAS默认逻辑正常执行
    Super::PostGameplayEffectExecute(Data);
    
    // 初始化效果属性结构体，用于封装源/目标的核心信息
    FEffectProperties Props;
    // 解析效果的源/目标属性到Props中（核心：为后续逻辑提供数据支撑）
    SetFEffectProperties(Data, Props);
    
    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
        GEngine->AddOnScreenDebugMessage(
    -1,                      // Key值，-1 表示自动分配唯一Key
    5.0f,                    // 显示时长（秒） 
    FColor::White,           // 文本颜色
    FString::Printf(TEXT("Change Health on : %s, health: %f"), 
        *Props.TargetAvatarActor->GetName(), 
        GetHealth()
    )
);
    }
    if (Data.EvaluatedData.Attribute == GetManaAttribute())
    {
        SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
    }
    // 【重要提示】：
    // 效果的产生场景不同，Props中的属性可能不全有效（如环境效果无SourceController）；
    // 后续使用Props中的属性（如SourceCharacter、TargetASC）时，必须通过IsValid()校验有效性，避免空指针崩溃。
}


void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor,OldVigor);
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

/**
 * @brief 生命值（Health）网络同步回调函数
 * 触发时机：服务器同步Health属性到客户端后，由UE自动调用（因注册时指定REPNOTIFY_Always，无条件触发）
 * @param OldHealth 同步前客户端本地的Health旧值（用于对比变更幅度，如计算掉血量）
 * 核心逻辑：通过GAS内置宏转发回调，触发属性变更的全局通知（供其他系统监听，如UI、技能逻辑）
 */
void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
    // GAMEPLAYATTRIBUTE_REPNOTIFY：GAS提供的属性同步通知宏，核心作用：
    // 1. 验证属性的有效性（确保是GAS注册的合法GameplayAttribute）
    // 2. 触发属性变更的全局事件（OnGameplayAttributeChanged），让其他监听该属性的系统（如UI组件、技能系统）响应
    // 3. 自动处理属性的"旧值→新值"切换逻辑，无需手动维护
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

/**
 * @brief 最大生命值（MaxHealth）网络同步回调函数
 * 触发时机：服务器同步MaxHealth属性到客户端后，由UE自动调用（无条件触发）
 * @param OldMaxHealth 同步前客户端本地的MaxHealth旧值（用于对比变更幅度，如升级增加的最大血量）
 * 核心逻辑：与OnRep_Health一致，通过GAS宏转发通知，确保全局系统感知最大生命值变更
 */
void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
    // 调用GAS内置宏，触发MaxHealth属性的全局变更通知
    // 后续可由UI系统监听该事件，刷新最大血量显示；或由技能系统调整基于最大血量的效果（如百分比回血技能）
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

//于上面的一致
void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}



void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}


