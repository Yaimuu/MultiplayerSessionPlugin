// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Interfaces/OnlineSessionInterface.h>
#include "OnlineSessionSettings.h"
#include "MultiplayerMenu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 DefaultNumberPublicConnections = 4, FString DefaultGameMode = FString(TEXT("FreeForAll")), FString DefaultLobbyPath = FString(TEXT("/MultiplayerSessions/Lobby")));

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	//
	// Blueprint callable functions
	//
	//UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	//void HostGameSession(int32 NumberPublicConnections, FName SessionName, FString Password, FString GameMode);
	//UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	//void JoinGameSession(FString Id);
	//UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	//void FindGameSessions();

	//
	// Callbacks for the custom delegates for the Menu class to bind callbacks to
	//
	//UFUNCTION()
	//void OnCreateSession(bool bWasSuccessful);
	//void OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	//void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	//UFUNCTION()
	//void OnDestroySession(bool bWasSuccessful);
	//UFUNCTION()
	//void OnStartSession(bool bWasSuccessful);

private:

	void MenuTearDown();

	// The subsystem designed to handle all online session functionality
	//class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	//int32 LastNumPublicConnections{ 4 };
	//FString LastGameMode{ (TEXT("FreeForAll")) };
	//FString LastLobbyPath{ (TEXT("")) };

	//TArray<FOnlineSessionSearchResult> LastSessionResults;
};
