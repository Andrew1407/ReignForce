// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MessageLogger/MessageLoggerWidget.h"
#include "Components/PanelWidget.h"
#include "UI/MessageLogger/GameMessageWidget.h"


UGameMessageWidget* UMessageLoggerWidget::AddMessageByType(const FText& Message, EGameMessageType MessageType)
{
    if (!IsValid(MessageContainer)) return nullptr;

    TSubclassOf<UGameMessageWidget>* TemplatePtr = MessageTemplates.Find(MessageType);
    if (!TemplatePtr) return nullptr;

    auto GameMessageWidget = CreateWidget<UGameMessageWidget>(GetWorld(), *TemplatePtr);
    if (!IsValid(GameMessageWidget)) return nullptr;
    GameMessageWidget->SetMessageText(Message);
    
    for (UWidget* Widget : MessageContainer->GetAllChildren())
    {
        if (Widget->IsA(UGameMessageWidget::StaticClass())) Widget->RemoveFromParent();
    }

    MessageContainer->AddChild(GameMessageWidget);
    return GameMessageWidget;
}