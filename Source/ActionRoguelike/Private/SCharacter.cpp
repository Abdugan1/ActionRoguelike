// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "SActionComponent.h"
#include "SPlayerAttributeComponent.h"
#include "SInteractionComponent.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


static TAutoConsoleVariable<float> CVarRageMultiplier{ TEXT("su.RageMultiplier"), 0.7f, TEXT("Rage multiplier per player hit for Attribute Component"), ECVF_Cheat };


ASCharacter::ASCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComp->bUsePawnControlRotation = true; // true
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComponent = CreateDefaultSubobject<USInteractionComponent>("InteractionComponent");

	AttributeComponent = CreateDefaultSubobject<USPlayerAttributeComponent>("AttributeComponent");

	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");

	GetCharacterMovement()->bOrientRotationToMovement = true; // true

	bUseControllerRotationYaw = false; // false
}


void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComponent->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}


FVector ASCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}


void ASCharacter::HealSelf(float Amount /* 100 */)
{
	AttributeComponent->ApplyHealthChange(this, Amount);
}


void ASCharacter::MoveForward(float X)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	AddMovementInput(ControlRot.Vector(), X);
}


void ASCharacter::MoveRight(float X)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	// X = Forward (Red)
	// Y = Right (Green)
	// Z = Up (Blue)

	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

	AddMovementInput(RightVector, X);
}


void ASCharacter::SprintStart()
{
	ActionComp->StartActionByName(this, "Sprint");
}


void ASCharacter::SprintStop()
{
	ActionComp->StopActionByName(this, "Sprint");
}


void ASCharacter::PrimaryAttack()
{
	ActionComp->StartActionByName(this, "PrimaryAttack");
}


void ASCharacter::BlackholeAttack()
{
	ActionComp->StartActionByName(this, "Blackhole");
}

void ASCharacter::DashAttack()
{
	ActionComp->StartActionByName(this, "Dash");
}


void ASCharacter::PrimaryInteract()
{
	if (InteractionComponent)
		InteractionComponent->PrimaryInteract();
}


void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth,
	float Delta)
{
	if (Delta < 0.0f)
	{
		// Hit Flash
		GetMesh()->SetScalarParameterValueOnMaterials("HitFlashTime", GetWorld()->TimeSeconds);

		// Apply rage change by factor. -Delta because it's damage, not heal
		const float RageMultiplier = CVarRageMultiplier.GetValueOnGameThread();
		AttributeComponent->ApplyRageChange(nullptr, -Delta * RageMultiplier); // @todo: Do I need an Instigator here?

		// Died
		if (NewHealth <= 0.0f)
		{
			APlayerController* PlayerController = Cast<APlayerController>(GetController());
			DisableInput(PlayerController);
		}
	}
}


//void ASCharacter::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	//// -- Rotation Visualization -- //
//	//const float DrawScale = 100.0f;
//	//const float Thickness = 5.0f;
//
//	//FVector LineStart = GetActorLocation();
//	//// Offset to the right of pawn
//	//LineStart += GetActorRightVector() * 100.0f;
//	//// Set line end in direction of the actor's forward
//	//FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
//	//// Draw Actor's Direction
//	//DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);
//
//	//FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
//	//// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
//	//DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f, 0, Thickness);
//
//}


void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Lookup", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("BlackholeAttack", IE_Pressed, this, &ASCharacter::BlackholeAttack);
	PlayerInputComponent->BindAction("DashAttack", IE_Pressed, this, &ASCharacter::DashAttack);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::SprintStop);
}

