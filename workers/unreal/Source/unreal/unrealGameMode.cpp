// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "unreal.h"
#include "Engine.h"
#include "unrealGameMode.h"
#include "unrealPlayerController.h"

#if UE_SERVER
const std::string WorkerType = "UnrealWorker";
#else
const std::string WorkerType = "UnrealClient";
#endif

#define ENTITY_BLUEPRINTS_FOLDER "/Game/EntityBlueprints"

AunrealGameMode* AunrealGameMode::Instance;

AunrealGameMode::AunrealGameMode()
{
    // Set the default player controller class
    PlayerControllerClass = AunrealPlayerController::StaticClass();

    // Don't spawn players automatically
    bStartPlayersAsSpectators = true;

    // No need for default pawn class
    DefaultPawnClass = nullptr;

    Instance = this;
}

AunrealGameMode::~AunrealGameMode()
{
    Instance = nullptr;
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
#if UE_SERVER
    MakeWindowed(10, 10);
#else
    MakeWindowed(1280, 720);
#endif
}

void AunrealGameMode::CreateWorkerConnection()
{
	//Commandline arguments
	const FString receptionistIpArgument = "receptionistIp";
	const FString receptionistPortArgument = "receptionistPort";
	const FString engineTypeArgument = "engineType";
	const FString engineIdArgument = "engineId";
	const FString linkProtocolArgument = "linkProtocol";

	//Parse commandline properties
	FString receptionistIp = "127.0.0.1";
	FParse::Value(FCommandLine::Get(), *receptionistIpArgument, receptionistIp);

	int port = 7777;
	FParse::Value(FCommandLine::Get(), *receptionistPortArgument, port);

	//engine type is used to deduce the engine platform
	//however this is not needed in unreal.
	FString engineType = WorkerType.c_str();
	FParse::Value(FCommandLine::Get(), *engineTypeArgument, engineType);

	std::string workerId;
	FString parsedWorkerId;
	if (!FParse::Value(FCommandLine::Get(), *engineIdArgument, parsedWorkerId))
	{
		workerId = WorkerType + TCHAR_TO_UTF8(*FGuid::NewGuid().ToString());
	}
	else
	{
		workerId = TCHAR_TO_UTF8(*parsedWorkerId);
	}

	FString parsedLinkProtocol = "RakNet";
	FParse::Value(FCommandLine::Get(), *linkProtocolArgument, parsedLinkProtocol);
	const auto linkProtocol = parsedLinkProtocol == "Tcp" ? worker::NetworkConnectionType::kTcp :  worker::NetworkConnectionType::kRaknet;

	//Log parsed input
	UE_LOG(LogTemp, Warning,
		   TEXT("PARSED: receptionistIp %s, port %d, engineType %s, workerId %s"),
		   *receptionistIp,
		   port,
		   *engineType,
		   workerId.c_str())

	//Setup connection
    using namespace improbable::unreal::core;
	FWorkerConnection::SetComponentMetaclasses(worker::GetComponentMetaclasses());
    Connection.Reset(new FWorkerConnection());
    Connection->GetView().OnDisconnect([](const worker::DisconnectOp& disconnect) {
        // GIsRequestingExit = true;
    });
    worker::ConnectionParameters Params;
	Params.Network.ConnectionType = linkProtocol;
	Params.Network.UseExternalIp = false;

    Params.WorkerType = WorkerType;
    Params.WorkerId = workerId;
    
    Connection->Connect(receptionistIp, port, Params, GetWorld());
}

void AunrealGameMode::RegisterEntityBlueprints()
{
    using namespace improbable::unreal::entity_spawning;
    Spawner.Reset(
        new FEntitySpawner(Connection->GetConnection(), Connection->GetView(), GetWorld()));
    TArray<UObject*> assets;
    if (EngineUtils::FindOrLoadAssetsByPath(TEXT(ENTITY_BLUEPRINTS_FOLDER), assets,
                                            EngineUtils::ATL_Class))
    {
        for (auto asset : assets)
        {
            UBlueprintGeneratedClass* blueprintGeneratedClass =
                Cast<UBlueprintGeneratedClass>(asset);
            if (blueprintGeneratedClass != nullptr)
            {
                FString blueprintName = blueprintGeneratedClass->GetName().LeftChop(
                    2);  // generated blueprint class names end with "_C"
                UE_LOG(LogTemp, Warning,
                       TEXT("Registering blueprint in entity spawner with name: %s"),
                       *blueprintName)
                Spawner->RegisterPrefabName(blueprintName, blueprintGeneratedClass);
            }
            else
            {
                UE_LOG(
                    LogTemp, Warning,
                    TEXT("Found asset in the EntityBlueprints folder which is not a blueprint: %s"),
                    *(asset->GetFullName()))
            }
        }
    }
    else
    {
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