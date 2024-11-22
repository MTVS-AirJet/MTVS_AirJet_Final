// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_StandbyWidget.h"
#include "KHS/K_PlayerController.h"
#include "KHS/K_GameInstance.h"
#include "KHS/K_GameState.h"
#include "KHS/K_PlayerList.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Widget.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "ImageUtils.h"
#include <MTVS_AirJet_Final.h>
#include "KHS/K_CesiumTeleportBox.h"

UK_StandbyWidget::UK_StandbyWidget(const FObjectInitializer& ObjectInitialize)
{
}

void UK_StandbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

    //LOG_S(Warning, TEXT("My Owner : %s"), *GetOwningPlayerPawn()->GetName());
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
    //GetWorld()->GetTimerManager().SetTimer(PlayerListUpdateTimer , this , &UK_StandbyWidget::PlayerListUpdateChildren , 1.0f , true);

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
    //GameState가져오기
    KGameState = Cast<AK_GameState>(UGameplayStatics::GetGameState(GetWorld()));
    if ( !KGameState )
    {
        LOG_S(Warning, TEXT("GameState doesn't exist"));
    }

    // 기존 PlayerList 초기화
    StandbyMenu_PlayerList->ClearChildren();


    // 각 플레이어의 이름과 인덱스를 사용해 K_PlayerList 항목 추가
    for ( int32 Index = 0; Index < KGameState->ConnectedPlayerNames.Num(); ++Index ) {
        FString PlayerName = KGameState->ConnectedPlayerNames[Index];

        LOG_S(Warning, TEXT("Set PlayerName(%s) in PlayerList"), *PlayerName);

        // K_PlayerList 인스턴스 생성
        PlayerList = CreateWidget<UK_PlayerList>(this , PlayerListFactory);
        if ( PlayerList ) 
        {
            // 플레이어 이름과 인덱스 설정
            PlayerList->PlayerNickName->SetText(FText::FromString(PlayerName));  // 플레이어 이름 텍스트 설정
            int tempidx = Index + 1;
            PlayerList->PlayerNum->SetText(FText::AsNumber(tempidx));  // 인덱스를 텍스트로 설정
            PlayerList->Setup(this , tempidx);  // 상위 위젯과 인덱스를 전달하여 초기화

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
            int tempidx = Index + 1;
            PlayerList->PlayerNum->SetText(FText::AsNumber(tempidx));  // 인덱스 설정
            PlayerList->Setup(this , tempidx);

            StandbyMenu_PlayerList->AddChild(PlayerList);
        }
    }
}

//게임시작 버튼 바인딩 함수
void UK_StandbyWidget::StartMission()
{
    PlayerController = Cast<AK_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld() , 0));
    if(PlayerController)
        PlayerController->SRPC_StartGame();
    //RemoveUI();
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
    //GameInstance 가져오기
	GameInstance = Cast<UK_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	 if ( !GameInstance ) {
		 UE_LOG(LogTemp , Error , TEXT("GameInstance를 가져오지 못했습니다."));
		 return;
	 }
     const auto& MissionData = GameInstance->MissionData;

    StandbyMenu_txt_Producer->SetText(FText::FromString(MissionData.producer));
    StandbyMenu_txt_MapName->SetText(FText::FromString(MissionData.mapName));
    StandbyMenu_txt_Latitude->SetText(FText::AsNumber(MissionData.latitude));
    StandbyMenu_txt_Longitude->SetText(FText::AsNumber(MissionData.longitude));

    // 미션 목록을 설정
    FString MissionList = TEXT("");
    for (auto Mission : MissionData.mission )
    {
        FString pinNo = FString::FromInt(Mission.pinNo + 1);
        FString commandNo;
        if(Mission.commandNo == 1){  commandNo = TEXT("편대 비행"); }
        else if(Mission.commandNo == 2) { commandNo = TEXT("지상 타격"); }      
                
        MissionList.Append(FString::Printf(TEXT("No.%s - %s\n") , *pinNo , *commandNo));
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

    //(추가) 위경도 변화예정값 담기(Cesium GeoReference)
    AK_CesiumTeleportBox* TelBox = Cast<AK_CesiumTeleportBox>(UGameplayStatics::GetActorOfClass(GetWorld(),AK_CesiumTeleportBox::StaticClass()));
    if ( TelBox )
    {
        TelBox->SetDestinationLogitudeLatitude(MissionData.longitude, MissionData.latitude);
    }


}

void UK_StandbyWidget::ReqMapInfo(FString MyRoomName)
{
    // 게임 인스턴스 가져와서 만들어둔 딜리게이트에 내 함수 바인딩
    auto* gi = UK_JsonParseLib::GetKGameInstance(GetWorld());

    if (!gi->MapInfoResUseDel.IsBound())
        gi->MapInfoResUseDel.BindUObject(this , &UK_StandbyWidget::ResMapInfo);
    //->이 델리게이트 바인딩을 통해 GameInstance에서 콜백이 들어올떄 
    //  이 델리게이트 변수가 BroadCast되면 이곳의 연결함수가 실행

    FMapInfoRequest data;
    data.mapName = MyRoomName;

    // 서버에 요청 시작 -> 1~4 단계를 거쳐 바인드한 함수에 데이터가 들어옴.
    UK_GameInstance::MyServerRequest<FMapInfoRequest>(GetWorld() , EEventType::MAPINFO , data);
}

void UK_StandbyWidget::ResMapInfo(const FMapInfoResponse& resData)
{
    // GEngine->AddOnScreenDebugMessage(-1 , 31.f , FColor::Yellow ,
    //                                  FString::Printf(
    //                                      TEXT("MapInfo Requset Call Back Data \n%s") , *resData.ResponseToString()));

    //인게임에서 사용할 미션데이터를 인스턴스에 저장
    FMissionDataRes md;
    md.producer = resData.producer;
    md.latitude = resData.latitude;
    md.longitude = resData.longitude;
    md.mapName = resData.mapName;
    md.mapImage = resData.mapImage;
    md.startPoint.x = resData.startPointX;
    md.startPoint.y = resData.startPointY;
    md.mission = resData.mission;

    GameInstance->InitializeMission(md);
    InitializeMissionData();
}
