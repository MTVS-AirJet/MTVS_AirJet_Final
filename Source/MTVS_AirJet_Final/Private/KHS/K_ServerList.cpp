// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_ServerList.h"
#include "KHS/K_ServerWidget.h"
#include "Components/Button.h"

void UK_ServerList::Setup(class UK_ServerWidget* InParent , uint32 InIndex)
{
	Parent = InParent;
	Index = InIndex;
	RowButton->OnClicked.AddDynamic(this , &UK_ServerList::OnClicked);
	RowButton->OnHovered.AddDynamic(this, &UK_ServerList::OnHovered);
}

void UK_ServerList::OnHovered()
{
	
	//OnHoveredReq();
}

void UK_ServerList::OnHoveredReq(const FMapInfoRequest& mapName)
{
	Parent->ReqSessionInfo(mapName);

}

void UK_ServerList::OnClicked()
{
	Parent->SelecetIndex(Index);
}