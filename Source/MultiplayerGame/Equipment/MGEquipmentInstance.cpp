// Copyright Artem Volovyi. All Rights Reserved.


#include "MGEquipmentInstance.h"

#include "MGFirstPersonCharacter.h"
#include "MGEquipmentDefinition.h"
#include "Net/UnrealNetwork.h"

void UMGEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Instigator);
	DOREPLIFETIME(ThisClass, SpawnedActorsThirdPerson);
	DOREPLIFETIME(ThisClass, SpawnedActorsFirstPerson);
}

UWorld* UMGEquipmentInstance::GetWorld() const
{
	if (const APawn* OwningPawn = GetPawn())
	{
		return OwningPawn->GetWorld();
	}

	return nullptr;
}

void UMGEquipmentInstance::Tick(float DeltaSeconds)
{
}

APawn* UMGEquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

void UMGEquipmentInstance::SpawnEquipmentActors(const TArray<FMGEquipmentActorToSpawn>& ActorsToSpawn)
{
	if (APawn* OwningPawn = GetPawn())
	{
		if (const AMGFirstPersonCharacter* Character = Cast<AMGFirstPersonCharacter>(OwningPawn))
		{
			for (const FMGEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
			{
				USceneComponent* AttachTarget = SpawnInfo.bFirstPersonView ? Character->GetFirstPersonArms() : Character->GetMesh();
				
				AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
				NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
				NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
				NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

				TArray<TObjectPtr<AActor>>& SpawnedActors = SpawnInfo.bFirstPersonView ? SpawnedActorsFirstPerson : SpawnedActorsThirdPerson;
				SpawnedActors.Add(NewActor);
			}
		}
	}
}

void UMGEquipmentInstance::SetEquipmentActorsVisibility(bool bIsVisible)
{
	for (AActor* Actor : SpawnedActorsThirdPerson)
	{
		if (Actor)
		{
			Actor->SetActorHiddenInGame(!bIsVisible);
		}
	}

	for (AActor* Actor : SpawnedActorsFirstPerson)
	{
		if (Actor)
		{
			Actor->SetActorHiddenInGame(!bIsVisible);
		}
	}
}


void UMGEquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActorsThirdPerson)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}

	SpawnedActorsThirdPerson.Empty();

	for (AActor* Actor : SpawnedActorsFirstPerson)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}

	SpawnedActorsFirstPerson.Empty();
}

void UMGEquipmentInstance::OnEquipped()
{
	// First person arms are hidden before equipping an item, so we start showing them
	if (const AMGFirstPersonCharacter* Character = Cast<AMGFirstPersonCharacter>(GetPawn()))
	{
		Character->GetFirstPersonArms()->SetHiddenInGame(false);
	}

	for (const AActor* Actor : SpawnedActorsFirstPerson)
	{
		USkeletalMeshComponent* ActorMesh = Cast<USkeletalMeshComponent>(Actor->GetRootComponent());
		ActorMesh->SetOnlyOwnerSee(true);
	}

	for (const AActor* Actor : SpawnedActorsThirdPerson)
	{
		USkeletalMeshComponent* ActorMesh = Cast<USkeletalMeshComponent>(Actor->GetRootComponent());
		ActorMesh->SetOwnerNoSee(true);
	}
}

void UMGEquipmentInstance::OnUnequipped()
{
}

void UMGEquipmentInstance::OnRep_Instigator()
{
}
