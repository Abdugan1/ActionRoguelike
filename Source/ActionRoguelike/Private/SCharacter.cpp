// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "SAttributeComponent.h"
#include "SInteractionComponent.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComp->bUsePawnControlRotation = true; // true
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComponent = CreateDefaultSubobject<USInteractionComponent>("InteractionComponent");

	AttributeComponent = CreateDefaultSubobject<USAttributeComponent>("AttributeComponent");

	GetCharacterMovement()->bOrientRotationToMovement = true; // true

	bUseControllerRotationYaw = false; // false

	CrosshairAttackLineSweepLenght = 5000;
	CrosshairAttackLineSweepShapeRadius = 20;
}


void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComponent->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
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


void ASCharacter::Attack(const TSubclassOf<AActor> &ProjectileClass)
{
	PlayAnimMontage(AttackAnim);

	UGameplayStatics::SpawnEmitterAttached(
		CastSpellEffect,
		GetMesh(),
		"Muzzle_01"
	);

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
	if (ensureAlways(ProjectileClass))
	{
		FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

		FHitResult Hit;

		FVector TraceStart = CameraComp->GetComponentLocation();
		TraceStart = TraceStart + CrosshairAttackLineSweepShapeRadius;
		FVector TraceEnd = TraceStart + (GetControlRotation().Vector() * CrosshairAttackLineSweepLenght);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;

		FCollisionShape Shape;
		Shape.SetSphere(CrosshairAttackLineSweepShapeRadius);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

		bool HitSomething = GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjectQueryParams, Shape, Params);

		FRotator ProjectileRotation;
		if (HitSomething)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit something"));
			ProjectileRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, Hit.ImpactPoint);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Does not hit something"));
			ProjectileRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, Hit.TraceEnd);
		}

		FTransform SpawnTM = FTransform(ProjectileRotation, HandLocation);

		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}
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


void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth,
	float Delta)
{
	UE_LOG(LogTemp, Log, TEXT("New Health %f, Delta %f"), NewHealth, Delta);
	if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials("HitFlashTime", GetWorld()->TimeSeconds);
		if (NewHealth <= 0.0f)
		{
			UE_LOG(LogTemp, Log, TEXT("Disable input!"));
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
}

