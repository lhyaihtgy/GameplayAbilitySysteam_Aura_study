// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

//只要激活了技能，就会调用这个函数，在这个函数中，我们首先从能力系统中获取玩家操作器，然后获取当前光标下的碰撞结果，最后将碰撞结果的位置广播出去。
//只要有技能激活，这个函数就会被调用，光标的位置就会被广播出去，可以在VaildData中获取到光标位置
void UTargetDataUnderMouse::Activate()
{
	const bool bIsLocallyController = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyController)
	{
		SendMouseCursorData();
	}
	else
	{
		

		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationKey = GetActivationPredictionKey();

		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle,ActivationKey).AddUObject(this,&UTargetDataUnderMouse::OnTargetDataReplicatedCallback);

		const bool bCallDelegate =  AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle,ActivationKey);
		if (!bCallDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(),GetActivationPredictionKey());

	if (DataHandle.Data.Num() > 0)
	{
		const FGameplayAbilityTargetData_SingleTargetHit* TargetData = static_cast<const FGameplayAbilityTargetData_SingleTargetHit*>(DataHandle.Data[0].Get());
	}
	

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
void UTargetDataUnderMouse::SendMouseCursorData()
{
	FScopedPredictionWindow ScopedPredictionWindow(AbilitySystemComponent.Get());

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Target,false,CursorHit);

	

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}


/**下面是ServerSetReplicatedTargetData这个函数的详细解释
        * 客户端向服务端同步技能目标数据的核心RPC调用
        * 解决"服务端激活技能时目标数据未就绪"的时序问题
        * 核心逻辑：将技能目标数据通过可靠RPC同步到服务端，绑定到指定技能实例，并通过预测键保证网络一致性
        
       AbilitySystemComponent->ServerSetReplicatedTargetData(
           // 1. 技能实例句柄：唯一标识当前要激活的技能实例
           //    GAS中每个技能（即使是同类型）被激活时都会生成唯一Handle
           //    作用：告诉服务端"这份目标数据是给哪一个技能实例用的"，避免多技能数据串用
           GetAbilitySpecHandle(),
       
           // 2. 技能激活预测键【核心】
           //    类型：FPredictionKey
           //    作用：
           //    - 标识"客户端预测激活该技能"的唯一操作ID
           //    - 服务端通过此键校验：客户端是否有权限预测这次技能激活
           //    - 若服务端校验失败，可通过此键精准回滚客户端的错误预测状态
           //    - 关联客户端"无延迟手感"和服务端"权威校验"的核心标识
           GetActivationPredictionKey(),
       
           // 3. 技能目标数据容器
           //    类型：FGameplayAbilityTargetDataHandle
           //    作用：存储技能激活必需的目标数据（如选中的敌人、释放位置、射线检测结果等）
           //    这是解决"技能激活缺数据"的核心载体，也是RPC要同步的核心内容
           DataHandle,
       
           // 4. 目标数据标签（可选）
           //    类型：FGameplayTag
           //    作用：对目标数据做分类/过滤（如标记"AOE目标""锁定目标"等）
           //    传空标签表示仅同步原始数据，不做额外分类
           FGameplayTag(),
       
           // 5. 作用域预测键【核心】
           //    类型：FPredictionKey
           //    与GetActivationPredictionKey()的分工：
           //    - ActivationPredictionKey：绑定"本次技能激活"的预测操作
           //    - ScopedPredictionKey：绑定"当前客户端预测上下文"的全局标识
           //    核心作用：
           //    1) 确保RPC请求在正确的预测上下文里发送（避免跨上下文的无效请求）
           //    2) 防重复执行：每个预测上下文的键唯一，相同键的重复请求会被服务端拒绝
           //    3) 时效性：服务端处理后会标记该键为"已确认"，超时未确认则客户端自动回滚
           AbilitySystemComponent->ScopedPredictionKey
       );
       
       /************************ 预测键（FPredictionKey）补充说明 ************************
        * 1. 预测键的核心设计目的：
        *    GAS采用"客户端预测+服务端权威"架构：
        *    - 客户端：按下技能键立即预测执行（显示特效/动画），保证无延迟手感
        *    - 服务端：最终校验并同步结果，若预测错误则通过预测键回滚客户端状态
        *    预测键就是客户端和服务端之间的"操作唯一身份证"
        *
        * 2. 两个预测键的协作逻辑：
        *    - 先通过ScopedPredictionKey确认"客户端当前处于合法预测阶段"
        *    - 再通过ActivationPredictionKey确认"这次技能激活是该阶段内的第N个合法操作"
        *    - 服务端双键校验通过后，才会接收并绑定目标数据
        *
        * 3. 预测键的关键特性：
        *    - 唯一性：每次预测操作生成新键，永不重复
        *    - 时效性：服务端处理后标记为"已确认"，超时未确认则客户端自动回滚
        *    - 容错性：校验失败时，精准回滚该键对应的预测操作（如撤销客户端已显示的技能特效）
        *****************************************************************************
        * 
 */
