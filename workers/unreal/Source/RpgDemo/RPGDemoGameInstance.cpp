// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "RpgDemo.h"
#include "RPGDemoGameInstance.h"

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
