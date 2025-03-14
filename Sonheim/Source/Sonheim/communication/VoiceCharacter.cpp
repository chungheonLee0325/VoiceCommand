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

	// if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::T))
	// {
	// 	SendWavFileAsJson(); 
	// 	// 'T' 키가 눌렸을 때 WAV 파일을 JSON으로 전송하는 함수 호출
	// }
}

// void AVoiceCharacter::SendWavFileAsJson()
// {
	// FString FilePath = FPaths::ProjectSavedDir() + TEXT("C:/AIPW/Sonheim/Saved/BouncedWavFiles/Test.wav");
	// // 저장된 WAV 파일의 경로를 지정 (Saved 폴더 내 Test.wav)
	//
	// TArray<uint8> BinaryData;
	// LoadWavFile(FilePath, BinaryData);
	// // 지정된 경로에서 WAV 파일을 로드하고 바이너리 데이터 배열에 저장
	//
	// FString Base64EncodedData;
	// EncodeToBase64(BinaryData, Base64EncodedData);
	// // 로드된 바이너리 데이터를 Base64 형식으로 인코딩
	//
	// FString JsonString = FString::Printf(TEXT("{\"audio\":\"%s\"}"), *Base64EncodedData);
	// // Base64 인코딩된 데이터를 JSON 문자열로 변환 (키: audio)
	//
	// SendJsonData(JsonString);
	// // JSON 데이터를 서버로 전송
// }

// Called to bind functionality to input
void AVoiceCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// void AVoiceCharacter::LoadWavFile(const FString& FilePath, TArray<uint8>& BinaryData)
// {
// 	// if (!FFileHelper::LoadFileToArray(BinaryData, *FilePath))
// 	// {
// 	// 	UE_LOG(LogTemp, Error, TEXT("Failed to load WAV file from path: %s"), *FilePath);
// 	// 	return;
// 	// 	// 지정된 경로에서 WAV 파일을 로드실패 
// 	// }
// }
//
// void AVoiceCharacter::EncodeToBase64(const TArray<uint8>& BinaryData, FString& Base64EncodedData)
// {
// 	// Base64EncodedData = FBase64::Encode(BinaryData);
// 	// // 바이너리 데이터를 Base64 형식으로 인코딩하여 문자열에 저장
// }
//
// void AVoiceCharacter::SendJsonData(const FString& JsonString)
// {
	// FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	// // HTTP 요청 객체 생성
	//
	// Request->SetURL(TEXT("http://192.168.20.53:8000/convert-audio"));
	// Request->SetURL(TEXT("http://192.168.20.53:72/convert-audio"));
	// // 요청할 서버의 URL 설정
	// //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! AI 분들께 아침에 받으면 넣어서 테스트 꼭 해보기!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//
	// Request->SetVerb(TEXT("POST"));
	// // HTTP 요청 방식을 POST로 설정
	//
	// Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	// // 요청 헤더에 Content-Type을 application/json으로 설정
	//
	// Request->SetContentAsString(JsonString);
	// // 요청 본문에 JSON 문자열 추가
	//
	// Request->OnProcessRequestComplete().BindLambda(
	// 	[](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bWasSuccessful)
	// 	{
	// 		if (bWasSuccessful && Res.IsValid())
	// 		{
	// 			UE_LOG(LogTemp, Log, TEXT("Successfully sent JSON data! Response: %s"), *Res->GetContentAsString());
	// 			// 요청이 성공
	// 		}
	// 		else
	// 		{
	// 			UE_LOG(LogTemp, Error, TEXT("Failed to send JSON data."));
	// 			// 요청이 실패
	// 		}
	// 	});

// };

