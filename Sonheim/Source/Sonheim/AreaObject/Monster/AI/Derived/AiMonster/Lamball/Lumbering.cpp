// Fill out your copyright notice in the Description page of Project Settings.


#include "Lumbering.h"

#include "Sonheim/Utilities/LogMacro.h"

void ULumbering::InitState()
{
	
}

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
}

void ULumbering::Execute(float dt)
{
	
}

void ULumbering::Exit()
{
	
}
