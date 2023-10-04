// Fill out your copyright notice in the Description page of Project Settings.


#include "Impact.h"

// Sets default values
AImpact::AImpact()
{
    ImapctEffect = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("ImapctEffect"));
    RootComponent = ImapctEffect;

}

void AImpact::BeginPlay()
{
    Super::BeginPlay();

    SetLifeSpan(ImapctEffect->GetFlipbookLength());
}

