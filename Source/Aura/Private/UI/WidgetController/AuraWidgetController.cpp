// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AuraWidgetController.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& params)
{
	PlayerController = params.PlayerController;
	PlayerState = params.PlayerState;
	AbilitySystemComponent = params.AbilitySystemComponent;
	AttributeSet = params.AttributSet;
}

void UAuraWidgetController::BroadcastInitialValues()
{
	
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}
