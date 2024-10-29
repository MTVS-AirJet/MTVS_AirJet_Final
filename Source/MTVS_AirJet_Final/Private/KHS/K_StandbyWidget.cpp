// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_StandbyWidget.h"
#include "KHS/K_PlayerController.h"
#include "KHS/K_GameInstance.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Widget.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "ImageUtils.h"
#include "KHS/K_PlayerList.h"

UK_StandbyWidget::UK_StandbyWidget(const FObjectInitializer& ObjectInitialize)
{
}

void UK_StandbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// GameInstance 가져오기
	GameInstance = Cast<UK_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if ( GameInstance )
	{
		// GameInstance에서 미션 데이터를 UI에 설정하는 함수 호출
		InitializeMissionData();
	}

    //// 포커스할 위젯 설정
    //if ( StandbyMenu ) // 포커스할 수 있는 주 위젯
    //{
    //    StandbyMenu->SetIsFocusable(true);  // StandbyMenu 위젯을 포커스 가능하도록 설정
    //    // 필요 시 다른 위젯들도 같은 방식으로 설정 가능
    //}

    // 1초마다 PlayerListUpdateChildren 호출
    GetWorld()->GetTimerManager().SetTimer(PlayerListUpdateTimer , this , &UK_StandbyWidget::PlayerListUpdateChildren , 1.0f , true);

}

bool UK_StandbyWidget::Initialize()
{
	Super::Initialize();

    // Button Binding =============================================
    if ( StandbyMenu_btn_Start ) //게임시작 버튼 바인딩
    {
        StandbyMenu_btn_Start->OnClicked.AddDynamic(this , &UK_StandbyWidget::StartMission);
    }
    if ( StandbyMenu_btn_Home ) //로비돌아가기 버튼 바인딩
    {
        StandbyMenu_btn_Home->OnClicked.AddDynamic(this , &UK_StandbyWidget::OpenLobbyLevel);
    }

	return true;
}

void UK_StandbyWidget::SetUI()
{
	Super::SetUI();
}

void UK_StandbyWidget::RemoveUI()
{
	Super::RemoveUI();
}

// PlayerList ScrollBox에 플레이어 정보를 Set하는 함수
void UK_StandbyWidget::SetPlayerList()
{
    // GameInstance 가져오기
    GameInstance = Cast<UK_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if ( !GameInstance ) {
        UE_LOG(LogTemp , Error , TEXT("GameInstance를 가져오지 못했습니다."));
        return;
    }

    // 기존 PlayerList 초기화
    StandbyMenu_PlayerList->ClearChildren();

    // 각 플레이어의 이름과 인덱스를 사용해 K_PlayerList 항목 추가
    for ( int32 Index = 0; Index < GameInstance->ConnectedPlayerNames.Num(); ++Index ) {
        FString PlayerName = GameInstance->ConnectedPlayerNames[Index];

        // K_PlayerList 인스턴스 생성
        PlayerList = CreateWidget<UK_PlayerList>(this , PlayerListFactory);
        if ( PlayerList ) {
            // 플레이어 이름과 인덱스 설정
            PlayerList->PlayerNickName->SetText(FText::FromString(PlayerName));  // 플레이어 이름 텍스트 설정
            PlayerList->PlayerNum->SetText(FText::AsNumber(Index));  // 인덱스를 텍스트로 설정
            PlayerList->Setup(this , Index);  // 상위 위젯과 인덱스를 전달하여 초기화

            // StandbyMenu_PlayerList에 추가
            StandbyMenu_PlayerList->AddChild(PlayerList);
        }
    }
}

// PlayerList 1초마다 업데이트
void UK_StandbyWidget::PlayerListUpdateChildren()
{
    PlayerController = Cast<AK_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld() , 0));
    if ( PlayerController->HasAuthority() )  // 서버에서만 PlayerList 업데이트 요청
    {
        ServerRequestPlayerListUpdate();
    }
}
// 서버에서 PlayerList 업데이트 요청
void UK_StandbyWidget::ServerRequestPlayerListUpdate_Implementation()
{
    if ( GameInstance )
    {
        GameInstance = Cast<UK_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        if ( !GameInstance ) return;

        // `ConnectedPlayerNames` 배열을 클라이언트에 전달
        ClientUpdatePlayerList(GameInstance->ConnectedPlayerNames);
    }
}
// 서버에서 PlayerList 업데이트 요청
bool UK_StandbyWidget::ServerRequestPlayerListUpdate_Validate()
{
	return true;
}
// 모든 클라이언트에서 PlayerList 업데이트
void UK_StandbyWidget::ClientUpdatePlayerList_Implementation(const TArray<FString>& PlayerNames)
{
    // `PlayerNames` 배열을 사용하여 StandbyMenu_PlayerList 업데이트
    StandbyMenu_PlayerList->ClearChildren();

    for ( int32 Index = 0; Index < PlayerNames.Num(); ++Index )
    {
        PlayerList = CreateWidget<UK_PlayerList>(this , PlayerListFactory);
        if ( PlayerList )
        {
            PlayerList->PlayerNickName->SetText(FText::FromString(PlayerNames[Index]));  // 플레이어 이름 설정
            PlayerList->PlayerNum->SetText(FText::AsNumber(Index));  // 인덱스 설정
            PlayerList->Setup(this , Index);

            StandbyMenu_PlayerList->AddChild(PlayerList);
        }
    }
}

//게임시작 버튼 바인딩 함수
void UK_StandbyWidget::StartMission()
{
}

//로비레벨로 돌아가는 함수
void UK_StandbyWidget::OpenLobbyLevel()
{
    // 현재 플레이어의 컨트롤러를 가져와서 K_PlayerController로 캐스팅
    PlayerController = Cast<AK_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld() , 0));
    if ( PlayerController )
    {
        // PlayerController의 TravelToLobbyLevel 호출하여 로비로 이동
        PlayerController->TravelToLobbyLevel();
    }
    else
    {
        UE_LOG(LogTemp , Warning , TEXT("Failed to cast PlayerController to AK_PlayerController"));
    }
}

// GameInstance의 MissionData로 위젯 설정하는 함수
void UK_StandbyWidget::InitializeMissionData()
{
    if ( !GameInstance )
    {
        UE_LOG(LogTemp , Error , TEXT("GameInstance is null in UK_StandbyWidget"));
        return;
    }

    // 미션 데이터를 UI에 설정
    const auto& MissionData = GameInstance->MissionData;

    StandbyMenu_txt_Producer->SetText(FText::FromString(MissionData.producer));
    StandbyMenu_txt_MapName->SetText(FText::FromString(MissionData.mapName));
    StandbyMenu_txt_Latitude->SetText(FText::AsNumber(MissionData.latitude));
    StandbyMenu_txt_Longitude->SetText(FText::AsNumber(MissionData.longitude));

    // 미션 목록을 설정
    FString MissionList;
    for ( const auto& Mission : MissionData.mission )
    {
        MissionList.Append(FString::Printf(TEXT("핀 번호: %d, 명령: %d\n") , Mission.pinNo , Mission.commandNo));
    }
    StandbyMenu_txt_CommandList->SetText(FText::FromString(MissionList));

    // 썸네일 이미지 설정
    TArray<uint8> MapImageData;
    if ( FBase64::Decode(MissionData.mapImage , MapImageData) )
    {
        UTexture2D* Texture = FImageUtils::ImportBufferAsTexture2D(MapImageData);
        if ( Texture )
        {
            StandbyMenu_img_Map->SetBrushFromTexture(Texture);
        }
        else
        {
            UE_LOG(LogTemp , Warning , TEXT("Failed to create texture from mission data image"));
        }
    }
    else
    {
        UE_LOG(LogTemp , Warning , TEXT("Failed to decode mission data image"));
    }

}
