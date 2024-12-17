// Copyright Epic Games, Inc. All Rights Reserved.

#include "hackaton_cityPickUpComponent.h"

Uhackaton_cityPickUpComponent::Uhackaton_cityPickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void Uhackaton_cityPickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &Uhackaton_cityPickUpComponent::OnSphereBeginOverlap);
}

void Uhackaton_cityPickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	Ahackaton_cityCharacter* Character = Cast<Ahackaton_cityCharacter>(OtherActor);
	if(Character != nullptr)
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);

		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
