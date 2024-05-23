// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerMenu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Online.h"
#include "OnlineSessionSettings.h"
#include <Interfaces/OnlineSessionInterface.h>

void UMultiplayerMenu::MenuSetup(int32 DefaultNumberPublicConnections, FString DefaultGameMode, FString DefaultLobbyPath)
{
	//LastNumPublicConnections = DefaultNumberPublicConnections;
	//LastGameMode = DefaultGameMode;
	//LastLobbyPath = FString::Printf(TEXT("%s?listen"), *DefaultLobbyPath);
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;
	//LastSessionResults.Empty();

	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController) {
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	//UGameInstance* GameInstance = GetGameInstance();
	//if (GameInstance) {
	//	MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	//}

	// Adding callbacks to the MultiplayerSessionsSubsystem delegates
	//if (MultiplayerSessionsSubsystem) {
	//	MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
	//	MultiplayerSessionsSubsystem->MultiplayerOnFindSessionComplete.AddUObject(this, &ThisClass::OnFindSession);
	//	MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
	//	MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
	//	MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	//}
}

bool UMultiplayerMenu::Initialize()
{
	if(!Super::Initialize())
		return false;

	return true;
}

void UMultiplayerMenu::NativeDestruct()
{
	MenuTearDown();

	Super::NativeDestruct();
}

//void UMultiplayerMenu::HostGameSession(int32 NbPublicConnections, FName SessionName, FString Password, FString GameMode)
//{
//	LastNumPublicConnections = NbPublicConnections;
//	if (MultiplayerSessionsSubsystem) {
//		MultiplayerSessionsSubsystem->CreateSession(LastNumPublicConnections, SessionName.ToString(), GameMode);
//	}
//}
//
//void UMultiplayerMenu::JoinGameSession(FString Id)
//{
//	if (MultiplayerSessionsSubsystem == nullptr) {
//		return;
//	}
//
//	if (LastSessionResults.Num() == 0) {
//		return;
//	}
//
//	for (auto Result : LastSessionResults) {
//		if (Result.GetSessionIdStr() == Id)
//		{
//			MultiplayerSessionsSubsystem->JoinSession(Result);
//			return;
//		}
//	}
//}
//
//void UMultiplayerMenu::FindGameSessions()
//{
//	if (MultiplayerSessionsSubsystem) {
//		MultiplayerSessionsSubsystem->FindSessions(10000);
//	}
//}

void UMultiplayerMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController) {
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}

//void UMultiplayerMenu::OnCreateSession(bool bWasSuccessful)
//{
//	if (bWasSuccessful) {
//		if (GEngine) {
//			GEngine->AddOnScreenDebugMessage(
//				-1,
//				15.f,
//				FColor::Yellow,
//				FString::Printf(TEXT("Session created successfully !"))
//			);
//		}
//
//		UWorld* World = GetWorld();
//		if (World) {
//			World->ServerTravel(LastLobbyPath);
//		}
//	}
//	else {
//		if (GEngine) {
//			GEngine->AddOnScreenDebugMessage(
//				-1,
//				15.f,
//				FColor::Red,
//				FString::Printf(TEXT("Failed to create session !"))
//			);
//		}
//	}
//}
//
//void UMultiplayerMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
//{
//	if (MultiplayerSessionsSubsystem == nullptr)
//		return;
//
//	LastSessionResults = SessionResults;
//
//	for (auto Result : SessionResults) {
//		FString Id = Result.GetSessionIdStr();
//		FString User = Result.Session.OwningUserName;
//		FString SettingsValues;
//		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValues);
//		if (GEngine) {
//			GEngine->AddOnScreenDebugMessage(
//				-1,
//				15.f,
//				FColor::Cyan,
//				FString::Printf(TEXT("Id : %s, User : %s, MatchType : %s"), *Id, *User, *SettingsValues)
//			);
//		}
//	}
//
//	if (GEngine && SessionResults.Num() == 0) {
//		GEngine->AddOnScreenDebugMessage(
//			-1,
//			15.f,
//			FColor::Red,
//			FString::Printf(TEXT("No session found !"))
//		);
//	}
//
//	/*if (!bWasSuccessful || SessionResults.Num() == 0) {
//		JoinButton->SetIsEnabled(true);
//	}*/
//}
//
//void UMultiplayerMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
//{
//	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
//	if (Subsystem)
//	{
//		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
//		if (SessionInterface.IsValid()) {
//			FString Address;
//			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);
//
//			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
//			if (PlayerController) {
//				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
//			}
//		}
//	}
//
//	/*if (Result != EOnJoinSessionCompleteResult::Success) {
//		JoinButton->SetIsEnabled(true);
//	}*/
//}
//
//void UMultiplayerMenu::OnDestroySession(bool bWasSuccessful)
//{
//	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
//	if (!Subsystem) {
//		return;
//	}
//
//
//}
//
//void UMultiplayerMenu::OnStartSession(bool bWasSuccessful)
//{
//	if (!bWasSuccessful) {
//		GEngine->AddOnScreenDebugMessage(
//			-1,
//			15.f,
//			FColor::Red,
//			FString::Printf(TEXT("Error occured during starting session !"))
//		);
//		return;
//	}
//
//	if (GEngine) {
//		GEngine->AddOnScreenDebugMessage(
//			-1,
//			15.f,
//			FColor::Yellow,
//			FString::Printf(TEXT("Session started successfully !"))
//		);
//	}
//}
