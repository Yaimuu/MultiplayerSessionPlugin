// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionLibrary.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Engine/World.h"

void UMultiplayerSessionLibrary::InitMultiplayer(int32 DefaultNumberPublicConnections, FString DefaultGameMode, FString DefaultLobbyPath)
{
	LastNumPublicConnections = DefaultNumberPublicConnections;
	LastGameMode = DefaultGameMode;
	LastLobbyPath = FString::Printf(TEXT("%s?listen"), *DefaultLobbyPath);
	
	LastSessionResults.Empty();
	SessionResultInfos.Empty();

	UWorld* World = GetWorld();
	UGameInstance* GameInstance = World->GetGameInstance();
	if (GameInstance) {
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	// Adding callbacks to the MultiplayerSessionsSubsystem delegates
	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionComplete.AddUObject(this, &ThisClass::OnFindSession);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

void UMultiplayerSessionLibrary::HostGameSession(int32 NbPublicConnections, FName SessionName, FString Password, FString GameMode)
{
	LastNumPublicConnections = NbPublicConnections;
	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->CreateSession(LastNumPublicConnections, SessionName.ToString(), GameMode);
	}
}

/**
* 
*/
bool UMultiplayerSessionLibrary::JoinGameSession(const FString& Id)
{
	if (MultiplayerSessionsSubsystem == nullptr) {
		return false;
	}

	if (LastSessionResults.Num() == 0) {
		return false;
	}

	for (auto Result : LastSessionResults) {
		if (Result.GetSessionIdStr() == Id)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return true;
		}
	}

	return false;
}

/**
* Finds all game sessions and stores it in SessionResultInfos
*/
void UMultiplayerSessionLibrary::FindGameSessions()
{
	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

/**
* Retrieves all sessions previously found
*/
TArray<FSessionInfo> UMultiplayerSessionLibrary::GetFoundSessions()
{
	return SessionResultInfos;
}

/**
* Retrieves a specific session among all sessions previously found
* @param Id - Id of the targeted session
* @param OutSessionInfo - Reference to stor the session if found
* @return Session has been found
*/
bool UMultiplayerSessionLibrary::GetFoundSessionById(const FString& Id, FSessionInfo& OutSessionInfo)
{
	if (SessionResultInfos.Num() == 0)
	{
		FindGameSessions();
	}

	for (const auto& Result : SessionResultInfos)
	{
		if (Result.SessionId == Id)
		{
			OutSessionInfo = Result;
			return true;
		}
	}

	return false;
}

bool UMultiplayerSessionLibrary::GetFoundSessionByUserName(const FString& UserName, FSessionInfo& OutSessionInfo)
{
	if (SessionResultInfos.Num() == 0)
	{
		FindGameSessions();
	}

	for (const auto& Result : SessionResultInfos)
	{
		if (Result.OwningUserName == UserName)
		{
			OutSessionInfo = Result;
			return true;
		}
	}

	return false;
}

FString UMultiplayerSessionLibrary::GetLocalPlayerUsername()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	
	if (Identity.IsValid())
	{
		// Get the first local player (assuming single player or first player)
		const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		if (LocalPlayer)
		{
			FUniqueNetIdRepl UserId = LocalPlayer->GetPreferredUniqueNetId();
			if (UserId.IsValid())
			{
				return Identity.Get()->GetPlayerNickname(*UserId);
			}
		}
	}

	// Return a default string if unable to get the username
	return FString(TEXT("Unknown"));
}

void UMultiplayerSessionLibrary::SetLocalPlayerUsername(FString UserName)
{
	LocalUserName = UserName;
}

void UMultiplayerSessionLibrary::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString::Printf(TEXT("Session created successfully !"))
			);
		}
		
		UWorld* World = GetWorld();
		if (World) {
			World->ServerTravel(LastLobbyPath);
		}

		FindGameSessions();
		GetFoundSessionByUserName(GetLocalPlayerUsername(), CurrentSessionInfo);
		
	}
	else {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString::Printf(TEXT("Failed to create session !"))
			);
		}
	}
}

void UMultiplayerSessionLibrary::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (MultiplayerSessionsSubsystem == nullptr)
		return;

	LastSessionResults = SessionResults;

	for (auto Result : SessionResults) {
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;
		FString UserId;
		if (Result.Session.OwningUserId.IsValid()) {
			UserId = Result.Session.OwningUserId.Get()->ToString();
		}
		FString GameMode;
		FString SessionName;
		Result.Session.SessionSettings.Get(FName("GameMode"), GameMode);
		Result.Session.SessionSettings.Get(FName("SessionName"), SessionName);

		FSessionInfo newSessionInfo = FSessionInfo(Id, SessionName, GameMode, UserId, User, Result.Session.NumOpenPrivateConnections, Result.Session.NumOpenPublicConnections);

		SessionResultInfos.Add(newSessionInfo);

		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Cyan,
				FString::Printf(TEXT("Id : %s, User : %s, GameMode : %s"), *Id, *User, *GameMode)
			);
		}
	}

	if (GEngine && SessionResults.Num() == 0) {
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Red,
			FString::Printf(TEXT("No session found !"))
		);
	}
}

void UMultiplayerSessionLibrary::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) {
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			UWorld* World = GetWorld();
			APlayerController* PlayerController = World->GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController) {
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UMultiplayerSessionLibrary::OnDestroySession(bool bWasSuccessful)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem) {
		return;
	}


}

void UMultiplayerSessionLibrary::OnStartSession(bool bWasSuccessful)
{
	if (!bWasSuccessful) {
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Red,
			FString::Printf(TEXT("Error occured during starting session !"))
		);
		return;
	}

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString::Printf(TEXT("Session started successfully !"))
		);
	}
}