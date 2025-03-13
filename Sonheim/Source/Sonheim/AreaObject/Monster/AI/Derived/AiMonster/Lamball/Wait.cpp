// Fill out your copyright notice in the Description page of Project Settings.


#include "Wait.h"

#include "Sonheim/AreaObject/Monster/BaseMonster.h"

void UWait::InitState()
{
}

void UWait::CheckIsValid()
{
}

void UWait::Enter()
{
	FLog::Log("UWait::Enter");
}

void UWait::Execute(float dt)
{
	if (m_Owner == nullptr || !m_Owner->GetResourceTarget())
	{
		//FLog::Log("UWait::return");
		return;
	}

	//FLog::Log("UWait::Execute");
	ChangeState(m_NextState);
}

void UWait::Exit()
{
}
