// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "RpgDemo.h"
#include "Engine.h"
#include "RpgDemoGameMode.h"
#include "RpgDemoPlayerController.h"
#include <improbable/player/heartbeat.h>
#include <improbable/player/heartbeat_receiver.h>
#include <improbable/common/transform.h>
#include "improbable/standard_library.h"

#if UE_SERVER
const std::string WorkerType = "UnrealWorker";
#else
const std::string WorkerType = "UnrealClient";
#endif

#define ENTITY_BLUEPRINTS_FOLDER "/Game/EntityBlueprints"

namespace {
worker::Entity GetPlayerEntityTemplate()
{
    const improbable::math::Coordinates initialPosition{ 1.0, 20.0, 0.0 };
    const worker::List<float> initialRoation{ 1.0f, 0.0f, 0.0f, 0.0f };

    const improbable::WorkerAttributeSet unrealWorkerAttributeSet{ {worker::Option<std::string>{"UnrealWorker"}} };
    const improbable::WorkerAttributeSet unrealClientAttributeSet{ {worker::Option<std::string>{"UnrealClient"}} };

    const improbable::WorkerRequirementSet workerRequirementSet{ {unrealWorkerAttributeSet} };
    const improbable::WorkerRequirementSet clientRequirementSet{ {unrealClientAttributeSet} };
    const improbable::WorkerRequirementSet globalRequirmentSet{ {unrealClientAttributeSet, unrealWorkerAttributeSet} };

    worker::Map<std::uint32_t, improbable::WorkerRequirementSet> componentAuthority;

    componentAuthority.emplace(improbable::common::Transform::ComponentId, clientRequirementSet);
    componentAuthority.emplace(improbable::player::Heartbeat::ComponentId, clientRequirementSet);
    componentAuthority.emplace(improbable::player::HeartbeatReceiver::ComponentId, workerRequirementSet);

    const improbable::ComponentAcl componentAcl(componentAuthority);

    worker::Entity playerTempalte;
    playerTempalte.Add<improbable::common::Transform>(improbable::common::Transform::Data{ initialPosition, initialRoation });
    playerTempalte.Add<improbable::player::Heartbeat>(improbable::player::Heartbeat::Data{});
    playerTempalte.Add<improbable::player::HeartbeatReceiver>(improbable::player::HeartbeatReceiver::Data{});
    playerTempalte.Add<improbable::EntityAcl>(improbable::EntityAcl::Data{globalRequirmentSet, componentAcl});
    return playerTempalte;
}
}  // ::

ARpgDemoGameMode* ARpgDemoGameMode::Instance;

ARpgDemoGameMode::ARpgDemoGameMode()
{
    // Set the default player controller class
    PlayerControllerClass = ARpgDemoPlayerController::StaticClass();

    // Don't spawn players automatically
    bStartPlayersAsSpectators = true;

    // No need for default pawn class
    DefaultPawnClass = nullptr;

    Instance = this;
}

ARpgDemoGameMode::~ARpgDemoGameMode()
{
    Instance = nullptr;
}

void ARpgDemoGameMode::StartPlay()
{
    AGameMode::StartPlay();
    ConfigureWindowSize();
    CreateWorkerConnection();
}

void ARpgDemoGameMode::Tick(float DeltaTime)
{
    AGameMode::Tick(DeltaTime);
    if(Connection->IsConnected())
    {
        Connection->ProcessEvents();
    }
}

void ARpgDemoGameMode::SpawnPlayer()
{
    auto& connection = Connection->GetConnection();
    auto& view = Connection->GetView();

    const std::uint32_t timeoutMillis = 500;
    const std::string entityType = "Player";

    const auto entityIdReservationRequestId = connection.SendReserveEntityIdRequest(timeoutMillis);

    view.OnReserveEntityIdResponse([&connection, entityIdReservationRequestId, entityType, timeoutMillis](const worker::ReserveEntityIdResponseOp& op)
    {
        if (op.RequestId == entityIdReservationRequestId && op.StatusCode == worker::StatusCode::kSuccess)
        {
            connection.SendCreateEntityRequest(GetPlayerEntityTemplate(), entityType, op.EntityId, timeoutMillis);
        } 
    });
}

void ARpgDemoGameMode::ConfigureWindowSize()
{
#if UE_SERVER
    MakeWindowed(10, 10);
#else
    MakeWindowed(1280, 720);
#endif
}

void ARpgDemoGameMode::CreateWorkerConnection()
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
        workerId = WorkerType + std::string{ TCHAR_TO_UTF8(*FGuid::NewGuid().ToString()) };
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

    FOnConnectedDelegate OnConnected;
    OnConnected.BindLambda([this](bool connected) {
        if (connected)
        {
            SpawnPlayer();
            RegisterEntityBlueprints();
        }
    });
    
    Connection->ConnectToReceptionistAsync(receptionistIp, port, Params, GetWorld(), OnConnected);
}

void ARpgDemoGameMode::RegisterEntityBlueprints()
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

void ARpgDemoGameMode::MakeWindowed(int32 Width, int32 Height)
{
    UGameUserSettings* Settings = GetGameUserSettings();
    if (Settings != nullptr)
    {
        Settings->SetFullscreenMode(EWindowMode::Type::Windowed);

        Settings->SetScreenResolution(FIntPoint(Width, Height));
        Settings->SaveSettings();
    }
}

UGameUserSettings* ARpgDemoGameMode::GetGameUserSettings()
{
    if (GEngine != nullptr)
    {
        return GEngine->GameUserSettings;
    }
    return nullptr;
}
