// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"

#include "PhysicsEngine/RadialForceComponent.h"
#include "SAttributeComponent.h"


ASExplosiveBarrel::ASExplosiveBarrel()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	RootComponent = StaticMesh;
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetCollisionProfileName("PhysicsActor");

	Material = CreateDefaultSubobject<UMaterial>("Material");
	StaticMesh->SetMaterial(0, Material);

	RadialForce = CreateDefaultSubobject<URadialForceComponent>("RadialForce");
	RadialForce->SetupAttachment(StaticMesh);
	RadialForce->Radius = 1000.0f;
	RadialForce->ImpulseStrength = 1000.0f;
	RadialForce->bImpulseVelChange = true;
}


void ASExplosiveBarrel::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Exploded"));
	Explode();
	if (OtherActor)
	{
		auto AttributeComponent = USAttributeComponent::GetAttributes(OtherActor);
		if (AttributeComponent)
		{
			AttributeComponent->ApplyHealthChange(this, -80);
		}
	}
}


void ASExplosiveBarrel::Explode()
{
	RadialForce->FireImpulse();
}


void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	StaticMesh->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnCollisionHit);
}
