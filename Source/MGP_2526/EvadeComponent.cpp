// Fill out your copyright notice in the Description page of Project Settings.


#include "EvadeComponent.h"
#include"MGP_2526Character.h"

// Sets default values for this component's properties
UEvadeComponent::UEvadeComponent()
	EvadeSectionName(FName(TEXT(""))),
	bIsEvading(false),
	EvadeDirection(EEvadeDirection::none),
	ResetEvadeDirectionTimeRate(0.1f)
{


}


// Called when the game starts
void UEvadeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UEvadeComponent : : SendMovementVector(FVector2D MovementVector)
{
	SetEvadeDirection(MovementVector);

	GetWorld()->GetTimerManager().SetTimer(ResetEvadeDirectionTimerHandle, this, &UEvadeComponent::ResetEvadeDirection, ResetEvadeDirectionTimeRate);
}

void UEvadeComponent::SetEvadeDirection(FVector2D MovementVector)
{
	if (MovementVector.X > 0)
	{
		EvadeDirection = EEvadeDirection::Evade_Right;
	}
	else if (MovementVector.X < 0)
	{
		EvadeDirection = EEvadeDirection::Evade_Left;
	}
	else if (MovementVector.Y > 0)
	{
		EvadeDirection = EEvadeDirection::Evade_Forward;
	}
	else if (MovementVector.Y < 0)
	{
		EvadeDirection = EEvadeDirection::Evade_Backward;
	}
	else
	{
		EvadeDirection = EEvadeDirection::none;
	}
}

void UEvadeComponent::Evade(AMGP_2526Character* MGP_2526Character)
{
	if (!MGP_2526Character) return;
	
	if (EvadeDirection == EEvadeDirection::none) return;

	bIsEvading = true;
	
	MainCharacterRef = MGP_2526Character;

	if (transparentMaterial)
	{
		if (MainCharacter)
		{
			MainCharacter->GetMesh()->SetMaterial(3, TransparentMaterial);
		}
	}

	if(mainCharacter)
	{
		UAnimInstance* AnimInstance = MainCharacter->GetMesh()->GetAnimInstance();

		switch (EvadeDirection)
		{

		case EEvadeDirection::none:
			EvadeSectionName = FName(TEXT("none"));
			break;

		case EEvadeDirection::Evade_Backward:
			EvadeSectionName = FName(TEXT("Evade_Backward"));
			break;

		case EEvadeDirection::Evade_Right:
			EvadeSectionName = FName(TEXT("Evade_Right"));
			break;

		case EEvadeDirection::Evade_Left:
			EvadeSectionName = FName(TEXT("Evade_Left"));
			break;

		case EEvadeDirection::Evade_Forward:
			EvadeSectionName = FName(TEXT("Evade_Forward"));
			break;
		}

		if(AnimInstance)
		{
			if (EvadeMontage)
			{
				AnimInstance->Montage_Play(EvadeMontage);
				AnimInstance->Montage_JumpToSection(EvadeSectionName);

				OnMontageBlendingInEnded.BindUObject(this, &UEvadeComponent::OnEvadeMontageBlendedInEnded);
				OnMontageBlendingOutStarted.BindUObject(this, &UEvadeComponent::OnEvadeMontageBlendingOut);
				
				AnimInstance->Montage_SetBlendedInDelegate(OnMontageBlendingInEnded, EvadeMontage);

				AnimInstance->Montage_SetBlendingOutDelegate(OnMontageBlendingOutStarted, EvadeMontage);
			}
		  
		}
	
	}
}

void UEvadeComponent : : OnEvadeMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (!MainCharacterRef) return;

	bIsEvading = false;

	EvadeDirection = EEvadeDirection::none;

	if (WeponMaterial)
	{
		if (MainCharacterRef)
		{
			MainCharacterRef->GetMesh()->SetMaterial(3, WeaponMaterial);
		}
	}
}

void UEvadeComponent::OnEvadeMontageBlendedInEnded(UAnimMontage* Montage)
{
	if (EvadeSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EvadeSound);
}

// Called every frame
void UEvadeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

