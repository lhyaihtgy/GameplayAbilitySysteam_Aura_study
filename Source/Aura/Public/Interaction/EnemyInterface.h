// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IEnemyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:	
	//对应的敌人完成这两个接口，playercontroller会检测鼠标下的Actor属性，同时会检测这两个接口是否重写，如果重写就会调用这两个接口，对于敌人来说，调用接口就会让敌人高亮
	virtual void HightLightEnemy() = 0;
	virtual void UnHightLightEnemy() = 0;
};
