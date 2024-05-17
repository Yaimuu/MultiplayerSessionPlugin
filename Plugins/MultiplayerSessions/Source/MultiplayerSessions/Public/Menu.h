// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Interfaces/OnlineSessionInterface.h>

#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString PathToLobby = FString(TEXT("/MultiplayerSessions/Lobby")));

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	//
	// Blueprint callable functions
	//
	UFUNCTION(BlueprintCallable)
	void HostGameSession(int32 NumberPublicConnections, FName SessionName, FString Password, FString GameMode);
	UFUNCTION(BlueprintCallable)
	void JoinGameSession(FString Id);
	UFUNCTION(BlueprintCallable, Category = "UI")
	void FindGameSessions(UWidget* SessionWidget);

	//
	// Callbacks for the custom delegates for the Menu class to bind callbacks to
	//
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

private:
	/*UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;*/

	//UPROPERTY(meta = (BindWidget))
	//UButton* JoinButton;

	//UFUNCTION()
	//void HostButtonClicked();

	/*UFUNCTION()
	void JoinButtonClicked();*/

	void MenuTearDown();

	// The subsystem designed to handle all online session functionality
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	int32 NumPublicConnections{ 4 };
	FString MatchType{ (TEXT("FreeForAll")) };
	FString LobbyPath{ (TEXT("")) };

	TArray<FOnlineSessionSearchResult> LastSessionResults;
};
