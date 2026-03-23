// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define CUSTOM_DEPTH_RED 250
//下面代码的作用是
#define ECC_Projectile ECollisionChannel::ECC_GameTraceChannel1
#define ECC_Target ECollisionChannel::ECC_GameTraceChannel2
#define ECC_ExcludePlayers ECollisionChannel::ECC_GameTraceChannel3

