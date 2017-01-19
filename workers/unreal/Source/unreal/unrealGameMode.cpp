// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "unreal.h"
#include "unrealGameMode.h"
#include "unrealPlayerController.h"
#include "Engine.h"

#ifdef UNREAL_CLIENT
const static bool IS_FSIM = false;
#else
const static bool IS_FSIM = true;
#endif

const std::string WorkerType = (IS_FSIM ? "UnrealFsim" : "UnrealClient");
#define ENTITY_BLUEPRINTS_FOLDER "/Game/EntityBlueprints"

AunrealGameMode::AunrealGameMode()
{
	// Set the default player controller class
	PlayerControllerClass = AunrealPlayerController::StaticClass();

	// Don't spawn players automatically
	bStartPlayersAsSpectators = true;

	// No need for default pawn class
	DefaultPawnClass = nullptr;
}

void AunrealGameMode::StartPlay()
{
	AGameMode::StartPlay();
	ConfigureWindowSize();
	CreateWorkerConnection();
	RegisterEntityBlueprints();
}

void AunrealGameMode::Tick(float DeltaTime)
{
	AGameMode::Tick(DeltaTime);
	Connection->ProcessEvents();
}

void AunrealGameMode::ConfigureWindowSize()
{
	if (IS_FSIM)
	{
		MakeWindowed(10, 10);
	}
	else
	{
		MakeWindowed(1280, 720);
	}
}

void AunrealGameMode::CreateWorkerConnection()
{
	using namespace improbable::unreal::core;
	FWorkerConnection::SetComponentMetaClasses(worker::GetComponentMetaclasses());
	Connection.Reset(new FWorkerConnection());
	Connection->GetView().OnDisconnect([](const worker::DisconnectOp& disconnect)
	{
		//GIsRequestingExit = true;
	});
	worker::ConnectionParameters Params;
	Params.WorkerType = WorkerType;
	Params.WorkerId = Params.WorkerType + TCHAR_TO_ANSI(*FGuid::NewGuid().ToString());
	Connection->Connect(Params, GetWorld());
}

void AunrealGameMode::RegisterEntityBlueprints()
{
	using namespace improbable::unreal::entity_spawning;
	Spawner.Reset(new FEntitySpawner(Connection->GetConnection(), Connection->GetView(), GetWorld()));
	TArray<UObject*> assets;
	if (EngineUtils::FindOrLoadAssetsByPath(TEXT(ENTITY_BLUEPRINTS_FOLDER), assets, EngineUtils::ATL_Class)) {
		for (auto asset : assets) {
			UBlueprintGeneratedClass* blueprintGeneratedClass = Cast<UBlueprintGeneratedClass>(asset);
			if (blueprintGeneratedClass != nullptr) {
				FString blueprintName = blueprintGeneratedClass->GetName().LeftChop(2); // generated blueprint class names end with "_C"
				UE_LOG(LogTemp, Warning, TEXT("Registering blueprint in entity spawner with name: %s"), *blueprintName)
					Spawner->RegisterPrefabName(blueprintName, blueprintGeneratedClass);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Found asset in the EntityBlueprints folder which is not a blueprint: %s"), *(asset->GetFullName()))
			}
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("No assets found in EntityBlueprints folder."))
	}
}

void AunrealGameMode::MakeWindowed(int32 Width, int32 Height)
{
	UGameUserSettings* Settings = GetGameUserSettings();
	if (Settings != nullptr)
	{
		Settings->SetFullscreenMode(EWindowMode::Type::Windowed);

		Settings->SetScreenResolution(FIntPoint(Width, Height));
		Settings->SaveSettings();
	}
}

UGameUserSettings* AunrealGameMode::GetGameUserSettings()
{
	if (GEngine != nullptr)
	{
		return GEngine->GameUserSettings;
	}
	return nullptr;
}