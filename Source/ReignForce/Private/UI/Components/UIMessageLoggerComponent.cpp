// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/UIMessageLoggerComponent.h"
#include "UI/MessageLogger/MessageLoggerWidget.h"
#include "UI/MessageLogger/GameMessageType.h"
#include "UI/MessageLogger/GameMessageWidget.h"


UUIMessageLoggerComponent::UUIMessageLoggerComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetZOrder = 1;
    MessageCleanup = 1;

	ErrorLoggerMessageSavingDuringRoundText = FText::FromString(TEXT("Can't save game progress during round."));
	ErrorLoggerMessageSavingOnPlayerDeadText = FText::FromString(TEXT("Can't save game progress if player's dead."));
	LoggerMessageSavingSuccessText = FText::FromString(TEXT("Game progress successfully saved."));
}

void UUIMessageLoggerComponent::BeginDestroy()
{
    if (IsValid(this) && IsValid(GetWorld()))
    {
        FTimerManager& TimerManager = GetWorld()->GetTimerManager();
        if (MessageCleanupTimerHandle.IsValid() && TimerManager.IsTimerActive(MessageCleanupTimerHandle))
            TimerManager.ClearTimer(MessageCleanupTimerHandle);
    }

    Super::BeginDestroy();
}

bool UUIMessageLoggerComponent::OpenGameMessageLogger()
{
    if (IsGameMessageLoggerOpened() || !IsValid(MessageLoggerWidgetClass)) return false;
    MessageLoggerWidget = CreateWidget<UMessageLoggerWidget>(GetWorld(), MessageLoggerWidgetClass);
    if (!IsValid(MessageLoggerWidget)) return false;
    MessageLoggerWidget->AddToViewport(WidgetZOrder);
    return true;
}

bool UUIMessageLoggerComponent::IsGameMessageLoggerOpened() const
{
    return IsValid(MessageLoggerWidget);
}

bool UUIMessageLoggerComponent::SetGameMessageLoggerVisibility(bool bVisible)
{
    bool bCanSet = IsGameMessageLoggerOpened();
    if (bCanSet)
    {
        const ESlateVisibility Visibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
        MessageLoggerWidget->SetVisibility(Visibility);
    }
    return bCanSet;
}

bool UUIMessageLoggerComponent::AddErrorLoggerMessageSavingDuringRound()
{
    if (!IsValid(MessageLoggerWidget)) return false;
	UGameMessageWidget* Message = MessageLoggerWidget->AddMessageByType(ErrorLoggerMessageSavingDuringRoundText, EGameMessageType::Failed);
    bool bCreated = IsValid(Message);
    if (bCreated) SetCleanupMessageTimer(Message);
    return bCreated;
}

bool UUIMessageLoggerComponent::AddErrorLoggerMessageSavingOnPlayerDead()
{
    if (!IsValid(MessageLoggerWidget)) return false;
	UGameMessageWidget* Message = MessageLoggerWidget->AddMessageByType(ErrorLoggerMessageSavingOnPlayerDeadText, EGameMessageType::Failed);
    bool bCreated = IsValid(Message);
    if (bCreated) SetCleanupMessageTimer(Message);
    return bCreated;
}

bool UUIMessageLoggerComponent::AddLoggerMessageSavingSuccess()
{
    if (!IsValid(MessageLoggerWidget)) return false;
	UGameMessageWidget* Message = MessageLoggerWidget->AddMessageByType(LoggerMessageSavingSuccessText, EGameMessageType::Success);
    bool bCreated = IsValid(Message);
    if (bCreated) SetCleanupMessageTimer(Message);
    return bCreated;
}

void UUIMessageLoggerComponent::SetCleanupMessageTimer(UGameMessageWidget* Message)
{
    if (!IsValid(MessageLoggerWidget)) return;

    UWorld* LevelWorld = GetWorld();
    bool bEnvValid = IsValid(this) && IsValid(LevelWorld);
    if (!bEnvValid) return;

    FTimerManager& TimerManager = LevelWorld->GetTimerManager();
    if (MessageCleanupTimerHandle.IsValid() && TimerManager.IsTimerActive(MessageCleanupTimerHandle))
        TimerManager.ClearTimer(MessageCleanupTimerHandle);

    TimerManager.SetTimer(MessageCleanupTimerHandle, [this, Message, LevelWorld]
    {
        bool bEnvValid = IsValid(this) && IsValid(LevelWorld);
        if (bEnvValid && IsValid(Message)) Message->RemoveFromParent();
    }, MessageCleanup, false);
}