// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "DiffResults.h"
#include "SAttributeComponent.h"
#include "SInteractionComponent.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComp->bUsePawnControlRotation = true; // true
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComponent = CreateDefaultSubobject<USInteractionComponent>("InteractionComponent");

	AttributeComponent = CreateDefaultSubobject<USAttributeComponent>("AttributeComponent");

	GetCharacterMovement()->bOrientRotationToMovement = true; // true

	bUseControllerRotationYaw = false; // false
} 

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

void ASCharacter::Attack(const TSubclassOf<AActor> &ProjectileClass)
{
	PlayAnimMontage(AttackAnim);

	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([this, &ProjectileClass]()
		{
			AttackElapsedTime(ProjectileClass);
		}
	);


	GetWorldTimerManager().SetTimer(TimerHandlePrimaryAttack, TimerCallback, 0.2f, false);
}

void ASCharacter::AttackElapsedTime(const TSubclassOf<AActor> &ProjectileClass)
{
	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	FHitResult Hit;

	FVector Start = CameraComp->GetComponentLocation();
	FVector End = Start + (GetControlRotation().Vector() * 1000);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	bool HitSomething = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectQueryParams);

	FRotator Rotation = HitSomething ? UKismetMathLibrary::FindLookAtRotation(HandLocation, Hit.ImpactPoint)
									 : UKismetMathLibrary::FindLookAtRotation(HandLocation, Hit.TraceEnd);

	FTransform SpawnTM = FTransform(Rotation, HandLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
}

void ASCharacter::PrimaryAttack()
{
	Attack(PrimaryProjectileClass);
}

void ASCharacter::BlackholeAttack()
{
	Attack(BlackholeProjectileClass);
}

void ASCharacter::DashAttack()
{
	Attack(DashProjectileClass);
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComponent)
		InteractionComponent->PrimaryInteract();
}

void ASCharacter::DoJump()
{
	Jump();
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//// -- Rotation Visualization -- //
	//const float DrawScale = 100.0f;
	//const float Thickness = 5.0f;

	//FVector LineStart = GetActorLocation();
	//// Offset to the right of pawn
	//LineStart += GetActorRightVector() * 100.0f;
	//// Set line end in direction of the actor's forward
	//FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
	//// Draw Actor's Direction
	//DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);

	//FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
	//// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
	//DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f, 0, Thickness);

}

// Called to bind functionality to input
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

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::DoJump);

	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
}

