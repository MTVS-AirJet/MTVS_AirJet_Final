// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_PlayerList.h"

void UK_PlayerList::Setup(UK_ServerWidget* InParent , uint32 InIndex)
{
	Parent = InParent;
	Index = InIndex;
}

bool UK_PlayerList::bIsCommander()
{
	return false;
}
