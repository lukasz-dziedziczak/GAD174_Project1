// Fill out your copyright notice in the Description page of Project Settings.


#include "Character2D.h"
#include "GAD174_Project1Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "LaserProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
ACharacter2D::ACharacter2D()
{
	// Character have a rifle at start
	bHasRifle = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	//// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	//Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	//Mesh1P->SetOnlyOwnerSee(true);
	//Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	//Mesh1P->bCastDynamicShadow = false;
	//Mesh1P->CastShadow = false;
	////Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	//Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	Weapon2D = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Weapon2D"));
	Weapon2D->SetupAttachment(FirstPersonCameraComponent);

	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}

// Called when the game starts or when spawned
void ACharacter2D::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}
	}

	Weapon2D->Stop();

	ammoCount = 9;
}

// Called to bind functionality to input
void ACharacter2D::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACharacter2D::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACharacter2D::Look);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ACharacter2D::Fire);
	}
}

void ACharacter2D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (flipbookTimer > 0)
	{
		flipbookTimer -= DeltaTime;
	}

	if (Weapon2D->IsPlaying() && flipbookTimer < 0)
	{
		Weapon2D->PlayFromStart();
		Weapon2D->Stop();
	}
}

void ACharacter2D::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ACharacter2D::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACharacter2D::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool ACharacter2D::GetHasRifle()
{
	return bHasRifle;
}

void ACharacter2D::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	/*if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetController()->GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(FireMappingContext);
	}*/
}

void ACharacter2D::Fire()
{
	//UE_LOG(LogTemp, Warning, TEXT("Weapon Fired"));
	if (ammoCount > 0)
	{
		ammoCount--;

		// Try and fire a projectile
		if (ProjectileClass != nullptr)
		{
			UWorld* const World = GetWorld();
			if (World != nullptr)
			{
				APlayerController* PlayerController = Cast<APlayerController>(GetController());
				const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = /*GetOwner()->*/GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// Spawn the projectile at the muzzle
				World->SpawnActor<ALaserProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}

		// Try and play the sound if specified
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// Play flipbook animation
		if (Weapon2D != nullptr)
		{
			Weapon2D->Play();
			flipbookTimer = Weapon2D->GetFlipbookLength();
		}

		OnWeaponFire.Broadcast();
	}
	else
	{
		//Reload();
		OnBeginReload.Broadcast();
	}
	
}

void ACharacter2D::Reload()
{
	ammoCount = 9;
}
