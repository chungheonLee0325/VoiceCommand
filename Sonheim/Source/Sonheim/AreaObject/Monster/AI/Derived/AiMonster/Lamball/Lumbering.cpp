// Fill out your copyright notice in the Description page of Project Settings.


#include "Lumbering.h"

#include "AITypes.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "Sonheim/AreaObject/Monster/BaseMonster.h"
#include "Sonheim/AreaObject/Monster/AI/Base/BaseAIController.h"
#include "Sonheim/GameObject/ResourceObject/BaseResourceObject.h"
#include "Sonheim/Utilities/LogMacro.h"

void ULumbering::InitState()
{}

void ULumbering::CheckIsValid()
{
	if (m_NextState == EAiStateType::None)
	{
		FLog::Log("Please Set m_NextState");
	}
}

void ULumbering::Enter()
{
	FLog::Log("ULumbering::Enter");

	Target = UGameplayStatics::GetActorOfClass(GetWorld(), ABaseResourceObject::StaticClass());
}

void ULumbering::Execute(float dt)
{

	if (!Target)
	{
		ChangeState(m_NextState);
		return;
	}
	
	switch (CheckState())
	{
	case 0:
		bIsMoving = true;
		MoveToLumber();
		break;
	case 1:
		bIsLumbering = true;
		Lumbering(dt);
		break;
	case 2:
		bIsMoving = true;
		MoveToStore();
		break;
	case 3:
		StoreLumber(dt);
		break;
	default:
		FLog::Log("Error");
		break;
	}
}

void ULumbering::Exit()
{}

int32 ULumbering::CheckState()
{
	return MonsterState;
}

void ULumbering::MoveToLumber()
{
	if (bIsMoving)
	{
		//FLog::Log("MoveToLumber");

		if (m_Owner->AIController)
		{
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalActor(Target);
			MoveRequest.SetAcceptanceRadius(20.0f);

			FNavPathSharedPtr NavPath;

			m_Owner->AIController->MoveTo(MoveRequest, &NavPath);

			// // Debug line
			// auto PathPoints = NavPath->GetPathPoints();
			// for (auto Point : PathPoints)
			// {
			// 	FVector Location = Point.Location;
			//
			// 	UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Red, 5.f, 1.5f);
			// }
		}
		//FLog::Log("dist",	FVector::Distance(m_Owner->GetActorLocation(), Target->GetActorLocation()));


		if (FVector::Distance(m_Owner->GetActorLocation(), Target->GetActorLocation()) <= 200.0f)
		{
			MonsterState = 1;
			bIsMoving = false;
			ActionTime = 0.f;
		}
	}
}

void ULumbering::Lumbering(float dt)
{
	if (bIsLumbering)
	{
		//FLog::Log("Lumbering");

		UBaseSkill* skill = m_Owner->GetSkillByID(100);

		if (m_Owner->CanCastSkill(skill, m_Owner) && m_Owner->GetResourceTarget())
		{
			//FLog::Log("Lumbering");
			m_Owner->CastSkill(skill, m_Owner);
		}

		ActionTime += dt;
		if (ActionTime > LumberingTime)
		{
			MonsterState = 2;
			bIsLumbering = false;
			ActionTime = 0.f;
		}
	}
}


void ULumbering::MoveToStore()
{
	if (bIsMoving)
	{
		FLog::Log("MoveToStore");

		//auto Target{UGameplayStatics::GetActorOfClass(GetWorld(), ABaseResourceObject::StaticClass())};

		if (m_Owner->AIController)
		{
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalLocation(FVector::ZeroVector);
			//MoveRequest.SetGoalActor(Target);
			MoveRequest.SetAcceptanceRadius(20.0f);

			FNavPathSharedPtr NavPath;

			m_Owner->AIController->MoveTo(MoveRequest, &NavPath);

			// // Debug line
			// auto PathPoints = NavPath->GetPathPoints();
			// for (auto Point : PathPoints)
			// {
			// 	FVector Location = Point.Location;
			//
			// 	UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Red, 5.f, 1.5f);
			// }
		}

		if (FVector::Distance(m_Owner->GetActorLocation(), FVector::ZeroVector) <= 200.0f)
		{
			MonsterState = 3;
			bIsMoving = false;
			ActionTime = 0.f;
		}
	}
}

void ULumbering::StoreLumber(float dt)
{
	ActionTime += dt;

	if (ActionTime > StoreTime)
	{
		MonsterState = 0;
		//bIsLumbering = false;
		ActionTime = 0.f;
	}
}
