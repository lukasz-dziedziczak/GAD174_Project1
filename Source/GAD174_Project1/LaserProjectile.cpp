// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Impact.h"

// Sets default values
ALaserProjectile::ALaserProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("SphereComp"));
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ALaserProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void ALaserProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	

	// spawn impact
	if (ImpactClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			/*FRotator SpawnRotation = World->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation();
			SpawnRotation.Add(0,90,0);*/
			FRotator SpawnRotation = OtherActor->GetActorRotation();

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			World->SpawnActor<AImpact>(ImpactClass, Hit.ImpactPoint, SpawnRotation);
			//UE_LOG(LogTemp, Warning, TEXT("Spawned Impact"));
		}

		
	}

	Destroy();
}

