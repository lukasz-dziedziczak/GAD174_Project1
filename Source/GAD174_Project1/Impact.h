// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperFlipbookComponent.h"
#include "Impact.generated.h"

UCLASS()
class GAD174_PROJECT1_API AImpact : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AImpact();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UPaperFlipbookComponent* ImapctEffect;

protected:
	virtual void BeginPlay();

};
