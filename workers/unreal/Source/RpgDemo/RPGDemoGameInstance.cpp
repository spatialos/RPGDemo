// Fill out your copyright notice in the Description page of Project Settings.

#include "RPGDemoGameInstance.h"
#include "RpgDemo.h"

URPGDemoGameInstance::URPGDemoGameInstance() : SpatialOSInstance()
{
}

URPGDemoGameInstance::~URPGDemoGameInstance()
{
}

improbable::unreal::core::FSpatialOS& URPGDemoGameInstance::GetSpatialOS()
{
    return SpatialOSInstance;
}
