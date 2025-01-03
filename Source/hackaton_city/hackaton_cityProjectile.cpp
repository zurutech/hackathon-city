// Copyright Epic Games, Inc. All Rights Reserved.

#include "hackaton_cityProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Public/WFCSubsystem.h"

Ahackaton_cityProjectile::Ahackaton_cityProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &Ahackaton_cityProjectile::OnHit);
	// set up a notification for when this component hits something blocking

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

void Ahackaton_cityProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                     FVector NormalImpulse, const FHitResult& Hit)
{
	auto* wfcSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UWFCSubsystem>();
	const TWeakObjectPtr model = wfcSubsystem->WFCModel.Get();
	const auto tileSize = model->TileSize;
	const FVector buildingLocation{
		FMath::FloorToFloat(Hit.Location.X / tileSize) * tileSize,
		FMath::FloorToFloat(Hit.Location.Y / tileSize) * tileSize,
		0
	};

	wfcSubsystem->OriginLocation = buildingLocation;
	wfcSubsystem->Collapse(10, 0);

	Destroy();
	
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}
