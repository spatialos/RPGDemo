// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "GameFramework/GameUserSettings.h"
#include "EntitySpawner.h"
#include "WorkerConnection.h"
#include "RpgDemoGameMode.generated.h"

//UENUM(BlueprintType, Category="SpatialOS")
//enum class EWorkerType : uint8
//{
//	Unspecified		UMETA(DisplayName = "Unspecified"),
//	UnrealWorker	UMETA(Displayname = "UnrealWorker"),
//	UnrealClient	UMETA(Displayname = "UnrealClient")
//};
//
//USTRUCT()
//struct FSpatialOSWorkerConfiguration
//{
//	GENERATED_BODY()
//
//public:
//	static EWorkerType WorkerTypeFromName(FString workerTypeName)
//	{
//		UEnum* workerTypeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EWorkerType"));
//		auto type = workerTypeEnum->GetValueByName(FName(*workerTypeName));
//		return type == INDEX_NONE ? EWorkerType::Unspecified : (EWorkerType)type;
//	}
//
//	static FString WorkerTypeNameFromType(EWorkerType type)
//	{
//		const UEnum* enumType = FindObject<UEnum>(ANY_PACKAGE, TEXT("EWorkerType"));
//		return enumType->GetEnumName((int32)type);
//
//		//return UEnum::GetValueAsString(TEXT("EWorkerType"), type);
//	}
//
//	FSpatialOSWorkerConfiguration()
//		: WorkerId(""),
//		WorkerType(EWorkerType::Unspecified)
//	{}
//
//	FSpatialOSWorkerConfiguration(EWorkerType type, FString id)
//		: WorkerId(id),
//		WorkerType(type)
//	{}
//
//	FString WorkerTypeName()
//	{
//		return WorkerTypeNameFromType(WorkerType);
//	}
//	
//	UPROPERTY()
//	FString WorkerId;
//
//	UPROPERTY()
//	EWorkerType WorkerType;
//};

UCLASS(minimalapi)
class ARpgDemoGameMode : public AGameMode
{
  GENERATED_BODY()

public:
  ARpgDemoGameMode();
  virtual ~ARpgDemoGameMode();

  static improbable::unreal::entity_spawning::FEntitySpawner* GetSpawner()
  {
    return Instance->Spawner.GetOwnedPointer();
  }

  /*UFUNCTION(BlueprintCallable, Category = "SpatialOS")
	  static EWorkerType SpatialWorkerType() { return Instance->Worker.WorkerType; }

  UPROPERTY(BluePrintReadWrite, EditDefaultsOnly, NoClear)
	EWorkerType workerType = EWorkerType::Unspecified;

  UPROPERTY(BluePrintReadWrite, EditDefaultsOnly, NoClear)
	  FString workerId = "";*/

private:
	//FSpatialOSWorkerConfiguration Worker;

	void ConfigureWorker();
  void StartPlay() override;
  void Tick(float DeltaTime) override;

  void SpawnPlayer();

  TAutoPtr<improbable::unreal::core::FWorkerConnection> Connection;
  TAutoPtr<improbable::unreal::entity_spawning::FEntitySpawner> Spawner;

  static void ConfigureWindowSize();
  void CreateWorkerConnection();
  void RegisterEntityBlueprints();

  static void MakeWindowed(int32 Width, int32 Height);
  static UGameUserSettings* GetGameUserSettings();

  static ARpgDemoGameMode* Instance;
};
