// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectAction.h"

#include "Sonheim/Utilities/LogMacro.h"

void USelectAction::InitState()
{
}

void USelectAction::CheckIsValid()
{
}

void USelectAction::Enter()
{
	FLog::Log("USelectAction::Enter");
}

void USelectAction::Execute(float dt)
{
	ChangeState(m_NextState);
}

void USelectAction::Exit()
{
}

void USelectAction::OnSkillCompleted()
{
	
}

void USelectAction::OnSkillCanceled()
{
	
}
