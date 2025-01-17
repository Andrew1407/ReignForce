// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameStates/Components/PlayerProgressionBalance.h"
#include "GameStates/RoundState.h"
#include "Weapons/WeaponType.h"
#include "ShooterGameState.generated.h"


class APlayerCharacter;
class AEnemyCharacter;
class AShooterCharacter;

class UUpgradesProgressStateComponent;
class UEnemySpawnerComponent;
class UEnemyEquipSystem;
class URoundDifficultyProgression;

class URoundProgressWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundStarted, bool, bStartedByPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundEnded, bool, bPlayerWin);

/**
 * 
 */
UCLASS()
class REIGNFORCE_API AShooterGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AShooterGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	#pragma region EVENTS

	UPROPERTY(BlueprintAssignable, Category = "ShooterGameState")
	FOnRoundStarted OnRoundStarted;

	UPROPERTY(BlueprintAssignable, Category = "ShooterGameState")
	FOnRoundEnded OnRoundEnded;

	#pragma endregion

	#pragma region PLAYER_CHARACTER_SAVE_DATA

	UFUNCTION(BlueprintPure)
	bool DoesPlayerHaveSavedProgress() const;

	UFUNCTION(BlueprintCallable)
	bool SavePlayerCharacterState(APlayerCharacter* Player);

	UFUNCTION(BlueprintCallable)
	bool LoadPlayerCharacterState(APlayerCharacter* Player);

	UFUNCTION(BlueprintCallable)
	bool ClearPlayerCharacterState();

	UFUNCTION(BlueprintPure)
	EWeaponType GetWeaponSlotFromLastSave() const { return PlayerSlotFromSave; }

	UFUNCTION(BlueprintPure)
	FPlayerProgressionBalance GetCurrentProgress() const;

	#pragma endregion

	#pragma region ENEMIES

	UFUNCTION(BlueprintPure)
	AEnemyCharacter* FindSpawnedEnemyByName(const FString& EnemyName) const;

	UFUNCTION(BlueprintCallable)
	bool ActivateEnemiesSpawnByDefaultAmount();

	UFUNCTION(BlueprintCallable)
	bool ActivateEnemiesSpawnByProgression();

	UFUNCTION(BlueprintCallable)
	void ClearLevelFromSpawnedEnemies(bool bDeadOnly = false);

	UFUNCTION(BlueprintCallable)
	void StopEnemiesAttackForTarget(AActor* Target, float DelaySec = .7f);

	#pragma endregion

	#pragma region LEVEL_STATE

	UFUNCTION(BlueprintCallable)
	void StartRound();

	UFUNCTION(BlueprintCallable)
	void RestartLevel();

	UFUNCTION(BlueprintCallable)
	bool EndRound(bool bWithSuccess = true);

	#pragma endregion

	FORCEINLINE UEnemySpawnerComponent* GetEnemySpawnerComponent() const { return EnemySpawnerComponent; }
	FORCEINLINE UEnemyEquipSystem* GetEnemyEquipSystem() const { return EnemyEquipSystem; }
	FORCEINLINE UUpgradesProgressStateComponent* GetUpgradesProgressStateComponent() const { return UpgradesProgressStateComponent; }

	FORCEINLINE ERoundState GetRoundState() const { return RoundState; }
	FORCEINLINE bool IsRoundGoing() const { return RoundState != ERoundState::None; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Save", meta = (AllowPrivateAccess = true))
	FString CharacterStateSlot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progress", meta = (AllowPrivateAccess = true))
	TObjectPtr<UUpgradesProgressStateComponent> UpgradesProgressStateComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemies", meta = (AllowPrivateAccess = true))
	TObjectPtr<UEnemySpawnerComponent> EnemySpawnerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemies", meta = (AllowPrivateAccess = true))
	TObjectPtr<UEnemyEquipSystem> EnemyEquipSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Difficulty", meta = (AllowPrivateAccess = true))
	TObjectPtr<URoundDifficultyProgression> RoundDifficultyProgression;

	UPROPERTY(BlueprintReadOnly, Category = "Round", meta = (AllowPrivateAccess = true))
	ERoundState RoundState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Round|Sounds", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<USoundBase>> RoundWinSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Round|Sounds", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<USoundBase>> RoundLooseSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Round|Sounds", meta = (AllowPrivateAccess = true, ClampMin = 0, UIMin = 0, Units = "Times"))
	float RoundSoundVolume;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	EWeaponType PlayerSlotFromSave;

	UFUNCTION()
	void EquipEnemyShooter(AShooterCharacter* Shooter);

	UFUNCTION()
	void OnEnemiesSpawned(bool bSuccess);

	UFUNCTION()
	void AddSkillsForRoundWin(bool bPlayerWin);

	UFUNCTION()
	void PlaySoundOnRoundEnded(bool bPlayerWin);

	UFUNCTION()
	void OnEnemyPerished(AShooterCharacter* Enemy, AActor* Cause);

	bool SpawnEnemies(int32 ToSpawn = -1);

	URoundProgressWidget* GetRoundProgressWidget() const;
};
