// Copyright Epic Games, Inc. All Rights Reserved.

#include "hackaton_cityCharacter.h"
#include "hackaton_cityProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HackatonCityDeveloperSettings.h"
#include "InputActionValue.h"
#include "WaveFunctionCollapseModel.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Public/WFCSubsystem.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// Ahackaton_cityCharacter

Ahackaton_cityCharacter::Ahackaton_cityCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

//////////////////////////////////////////////////////////////////////////// Input

void Ahackaton_cityCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void Ahackaton_cityCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &Ahackaton_cityCharacter::GenerateCity);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &Ahackaton_cityCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &Ahackaton_cityCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void Ahackaton_cityCharacter::BeginPlay()
{
	Super::BeginPlay();


	auto* wfcSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UWFCSubsystem>();
	
	//wfcSubsystem->WFCModel = LoadObject<UWaveFunctionCollapseModel>(this, TEXT("/WaveFunctionCollapse/Sample_Buildings/WFCM_Sample_Buildings.WFCM_Sample_Buildings"));
	//wfcSubsystem->WFCModel = LoadObject<UWaveFunctionCollapseModel>(this, TEXT("/WaveFunctionCollapse/Sample_Pipes/WFCM_Sample_Pipes.WFCM_Sample_Pipes"));
	auto settings{GetDefault<UHackatonCityDeveloperSettings>()};
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->MaxWalkSpeed = settings->Speed;
	Speed = settings->Speed;
	wfcSubsystem->WFCModel = Cast<UWaveFunctionCollapseModel>(settings->BaseModel.TryLoad());
	settings->PopulateModel(wfcSubsystem->WFCModel);

	TMap<FWaveFunctionCollapseOption, FWaveFunctionCollapseAdjacencyToOptionsMap> constraints = wfcSubsystem->WFCModel->Constraints;
	constraints.Add(FWaveFunctionCollapseOption::EmptyOption, FWaveFunctionCollapseAdjacencyToOptionsMap{});
	wfcSubsystem->Resolution = settings->WFCResolution;
}


void Ahackaton_cityCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y * Speed);
		AddMovementInput(GetActorRightVector(), MovementVector.X * Speed);
	}
}

void Ahackaton_cityCharacter::Look(const FInputActionValue& Value)
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

void Ahackaton_cityCharacter::GenerateCity(const FInputActionValue&)
{
	Super::Jump();
}
