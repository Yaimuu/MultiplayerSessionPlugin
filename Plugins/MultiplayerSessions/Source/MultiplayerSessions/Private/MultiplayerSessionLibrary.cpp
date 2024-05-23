// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionLibrary.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/World.h"

void UMultiplayerSessionLibrary::InitMultiplayer(int32 DefaultNumberPublicConnections, FString DefaultGameMode, FString DefaultLobbyPath)
{
	LastNumPublicConnections = DefaultNumberPublicConnections;
	LastGameMode = DefaultGameMode;
	LastLobbyPath = FString::Printf(TEXT("%s?listen"), *DefaultLobbyPath);
	
	LastSessionResults.Empty();

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

void UMultiplayerSessionLibrary::JoinGameSession(FString Id)
{
	if (MultiplayerSessionsSubsystem == nullptr) {
		return;
	}

	if (LastSessionResults.Num() == 0) {
		return;
	}

	for (auto Result : LastSessionResults) {
		if (Result.GetSessionIdStr() == Id)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}
}

void UMultiplayerSessionLibrary::FindGameSessions()
{
	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
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
		FString SettingsValues;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValues);
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Cyan,
				FString::Printf(TEXT("Id : %s, User : %s, MatchType : %s"), *Id, *User, *SettingsValues)
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