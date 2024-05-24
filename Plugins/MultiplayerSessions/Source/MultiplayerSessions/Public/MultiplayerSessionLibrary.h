// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <Interfaces/OnlineSessionInterface.h>
#include "MultiplayerSessionLibrary.generated.h"

USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()

public:

	FSessionInfo() 
		: SessionId(""), SessionName(""), SessionGameMode(""), 
		OwningUserId(""), OwningUserName(""), 
		NumOpenPrivateConnections(0), NumOpenPublicConnections(0)
	{}

	FSessionInfo(FString SessionId, FString SessionName, FString SessionGameMode, FString OwningUserId, FString OwningUserName, int32 NumOpenPrivateConnections, int32 NumOpenPublicConnections)
		: SessionId(SessionId), SessionName(SessionName), SessionGameMode(SessionGameMode),
		OwningUserId(OwningUserId), OwningUserName(OwningUserName),
		NumOpenPrivateConnections(NumOpenPrivateConnections), NumOpenPublicConnections(NumOpenPublicConnections)
	{}

	/** Owner of the session */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionInfo")
	FString OwningUserId;
	/** Owner name of the session */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionInfo")
	FString OwningUserName;
	/** Id of the session */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionInfo")
	FString SessionId;
	/** Name of the session */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionInfo")
	FString SessionName;
	/** GameMode of the session */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionInfo")
	FString SessionGameMode;
	/** The number of private connections that are available (read only) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionInfo")
	int32 NumOpenPrivateConnections;
	/** The number of publicly available connections that are available (read only) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionInfo")
	int32 NumOpenPublicConnections;
};

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void InitMultiplayer(int32 DefaultNumberPublicConnections = 4, FString DefaultGameMode = FString(TEXT("FreeForAll")), FString DefaultLobbyPath = FString(TEXT("/MultiplayerSessions/Lobby")));

protected:

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void HostGameSession(int32 NumberPublicConnections, FName SessionName, FString Password, FString GameMode);
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	bool JoinGameSession(const FString& Id);
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void FindGameSessions();
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	TArray< FSessionInfo> GetFoundSessions();
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	bool GetFoundSessionById(const FString& Id, FSessionInfo& OutSessionInfo);
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	bool GetFoundSessionByUserName(const FString& UserName, FSessionInfo& OutSessionInfo);
	// Function to get the username
	UFUNCTION(BlueprintCallable, Category = "Player")
	FString GetLocalPlayerUsername();
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetLocalPlayerUsername(FString UserName);

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

	// The subsystem designed to handle all online session functionality
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	int32 LastNumPublicConnections{ 4 };
	FString LastGameMode{ (TEXT("FreeForAll")) };
	FString LastLobbyPath{ (TEXT("")) };

	FString LocalUserName;

	TArray<FOnlineSessionSearchResult> LastSessionResults;
	TArray<FSessionInfo> SessionResultInfos;
	FSessionInfo CurrentSessionInfo;
};
