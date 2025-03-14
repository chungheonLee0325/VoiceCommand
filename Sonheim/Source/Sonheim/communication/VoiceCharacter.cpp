// Fill out your copyright notice in the Description page of Project Settings.


#include "VoiceCharacter.h"
#include "http.h"

// Sets default values
AVoiceCharacter::AVoiceCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AVoiceCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVoiceCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// T키를 눌렀을때 Test 
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::T))
	{
		SendWavFileDirectly();
	}

}

void AVoiceCharacter::SendWavFileDirectly()
{
	FString FilePath = TEXT("BouncedWavFiles/Test.wav");
	// 절대 경로로 WAV 파일 위치 지정

	TArray<uint8> BinaryData;
	LoadWavFileBinary(FilePath, BinaryData);
	// 지정된 경로에서 WAV 파일 로드

	if (BinaryData.Num() > 0)
	{
		SendBinaryDataToServer(BinaryData);
		// 로드된 바이너리 데이터를 서버로 전송
	}
}


// Called to bind functionality to input
void AVoiceCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AVoiceCharacter::LoadWavFileBinary(const FString& FilePath, TArray<uint8>& BinaryData)
{
	if (!FFileHelper::LoadFileToArray(BinaryData, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load WAV file from path: %s"), *FilePath);
		return; 
		// 파일 로드 실패 시 로그 출력
	}

	UE_LOG(LogTemp, Log, TEXT("Successfully loaded WAV file: %s"), *FilePath);
}

void AVoiceCharacter::SendBinaryDataToServer(const TArray<uint8>& BinaryData)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	// HTTP 요청 객체 생성

	Request->SetURL(TEXT("http://192.168.20.56:72/generate_Json"));
	// 서버 URL 설정

	Request->SetVerb(TEXT("POST"));
	// HTTP 요청 방식을 POST로 설정

	Request->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));
	// 요청 헤더에 Content-Type 설정 (바이너리 데이터 전송)

	Request->SetContent(BinaryData);
	// 요청 본문에 바이너리 데이터 추가

	Request->OnProcessRequestComplete().BindLambda(
		[](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bWasSuccessful)
		{
			if (bWasSuccessful && Res.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("Successfully sent WAV file! Response: %s"), *Res->GetContentAsString());
				// 성공 시 서버 응답 내용 로그 출력
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to send WAV file. Response Code: %d"), Res.IsValid() ? Res->GetResponseCode() : -1);
				// 실패 시 에러 로그 출력
			}
		});

	Request->ProcessRequest();   
	// HTTP 요청 실행
}








