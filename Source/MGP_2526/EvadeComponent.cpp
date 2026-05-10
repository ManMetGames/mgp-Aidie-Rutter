// Fill out your copyright notice in the Description page of Project Settings.


#include "EvadeComponent.h"
#include"MGP_2526Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UEvadeComponent::UEvadeComponent() :
	bIsEvading(false),
	EvadeDirection(EEvadeDirection::None),
	EvadeSectionName(FName(TEXT("")))
	//ResetEvadeDirectionTimeRate(0.1f)
{


}


// Called when the game starts
void UEvadeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UEvadeComponent :: SendMovementVector(FVector2D MovementVector)
{
	SetEvadeDirection(MovementVector);

	//GetWorld()->GetTimerManager().SetTimer(ResetEvadeDirectionTimerHandle, this, &UEvadeComponent::ResetEvadeDirection, ResetEvadeDirectionTimeRate);
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
		EvadeDirection = EEvadeDirection::None;
	}
}

void UEvadeComponent::Evade(AMGP_2526Character* MainCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("EVADE FUNCTION CALLED"));

	if (!MainCharacter) return;
	
	if (EvadeDirection == EEvadeDirection::None) return;

	const float EvadeCost = 25.f;

	if (!MainCharacter->HasEnoughStamina(EvadeCost))
	{
		UE_LOG(LogTemp, Warning, TEXT("NOT ENOUGH STAMINA"));
		return;
	}

	MainCharacter->ConsumeStamina(EvadeCost);

	bIsEvading = true;
	
   MainCharacterRef = MainCharacter;

    if (TransparentMaterial)
	{
		if (MainCharacter)
		{
			MainCharacter->GetMesh()->SetMaterial(3, TransparentMaterial);
		}
	}

	if(MainCharacter)
	{
		UAnimInstance* AnimInstance = MainCharacter->GetMesh()->GetAnimInstance();

		switch (EvadeDirection)
		{

		case EEvadeDirection::None:
			EvadeSectionName = "";
			break;

		case EEvadeDirection::Evade_Backward:
			EvadeSectionName = "Evade_Backward";
			break;

		case EEvadeDirection::Evade_Right:
			EvadeSectionName = "Evade_Right";
			break;

		case EEvadeDirection::Evade_Left:
			EvadeSectionName = "Evade_Left";
			break;

		case EEvadeDirection::Evade_Forward:
			EvadeSectionName ="Evade_Forward";
			break;
		}

        if(AnimInstance)
		{
			if (EvadeMontage)
			{
				UE_LOG(LogTemp, Warning, TEXT("Montage: %s | Section: %s"),
					*GetNameSafe(EvadeMontage),
					*EvadeSectionName.ToString());

				AnimInstance->Montage_Play(EvadeMontage);
				UE_LOG(LogTemp, Warning, TEXT("PLAYING MONTAGE"));
				AnimInstance->Montage_JumpToSection(EvadeSectionName);

				OnMontageBlendingInEnded.BindUObject(this, &UEvadeComponent::OnEvadeMontageBlendedInEnded);
				OnMontageBlendingOutStarted.BindUObject(this, &UEvadeComponent::OnEvadeMontageBlendingOut);
				
				AnimInstance->Montage_SetBlendedInDelegate(OnMontageBlendingInEnded, EvadeMontage);

				AnimInstance->Montage_SetBlendingOutDelegate(OnMontageBlendingOutStarted, EvadeMontage);
			}
		  
		}
	
	}
}

void UEvadeComponent :: OnEvadeMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (!MainCharacterRef) return;

	bIsEvading = false;

	EvadeDirection = EEvadeDirection::None;

	//if (WeponMaterial)
	//{
	//	if (MainCharacterRef)
	//	{
	//		MainCharacterRef->GetMesh()->SetMaterial(3, WeaponMaterial);
	//	}
	//}
}

void UEvadeComponent::OnEvadeMontageBlendedInEnded(UAnimMontage* Montage)
{
	if (EvadeSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EvadeSound);
	}
}

//void UEvadeComponent ::ResetEvadeDirection()
//{
//	EvadeDirection = EEvadeDirection::None;
//}


