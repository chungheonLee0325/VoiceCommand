// Fill out your copyright notice in the Description page of Project Settings.


#include "VoiceCharacter.h"
#include "http.h"
#include "JsonObjectConverter.h"
#include "Sonheim/ResourceManager/SonheimGameType.h"

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
	

	// T키를 눌렀을 때 WAV 파일 전송 테스트
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::T))
	{
		SendWavFileDirectly();
	}

}


// Called to bind functionality to input
void AVoiceCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


// WAV 파일을 로드하고 바이너리 데이터로 전환
void AVoiceCharacter::LoadWavFileBinary(const FString& FilePath, TArray<uint8>& BinaryData)
{
	// WAV 파일을 로드
	if (!FFileHelper::LoadFileToArray(BinaryData, *FilePath))
	{
		// 로드 실패 시 로그 출력
		UE_LOG(LogTemp, Error, TEXT("Failed to load WAV file from path: %s"), *FilePath);
		return;
	}

	// 로드 성공 시 로그 출력
	UE_LOG(LogTemp, Log, TEXT("Successfully loaded WAV file: %s"), *FilePath);
	
}


TArray<uint8> AVoiceCharacter::FStringToUint8(FString str)
{
	TArray<uint8> outBytes;

	FTCHARToUTF8 converted(*str);
	outBytes.Append(reinterpret_cast<const uint8*>(converted.Get()), converted.Length());

	return outBytes;
}

// WAV 파일을 multipart/form-data 형식으로 전송
void AVoiceCharacter::SendWavFileAsFormData(const TArray<uint8>& BinaryData)
{

	// boundary 설정
	FString Boundary = "---------------------------boundary";
	FString FilePath = TEXT("Test.wav"); // WAV 파일 이름
	FString Key = TEXT("file"); // key 이름

	// 요청 본문 생성
	FString Body;
	Body += TEXT("--") + Boundary + TEXT("\r\n");
	Body += TEXT("Content-Disposition: form-data; name=\"") + Key + TEXT("\"; filename=\"") + FilePath + TEXT("\"\r\n");
	Body += TEXT("Content-Type: audio/wav\r\n\r\n");

	// WAV 파일 바이너리 데이터 추가
	TArray<uint8> FullData;
	FullData.Append(FStringToUint8(Body));
	FullData.Append(BinaryData);

	// boundary 끝 표시
	FString EndBoundary = TEXT("\r\n--") + Boundary + TEXT("--\r\n");
	FullData.Append(FStringToUint8(EndBoundary));

	// 요청 생성
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("192.168.20.56:72/convert-audio")); // 서버 URL
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary=") + Boundary);
	Request->SetContent(FullData);

	// 요청 완료 콜백 설정
	Request->OnProcessRequestComplete().BindLambda(
		[](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bWasSuccessful)
		{
			if (bWasSuccessful && Res.IsValid())
			{
				FAIVoiceOrder InfoArray;
				FString string = *Res->GetContentAsString();

				// ToDo : 처리
				FJsonObjectConverter::JsonObjectStringToUStruct(string, &InfoArray);
				
				// 성공 시 로그 출력
				UE_LOG(LogTemp, Log, TEXT("Successfully sent WAV file! Response: %s"), *Res->GetContentAsString());
			}
			else
			{
				// 실패 시 로그 출력
				UE_LOG(LogTemp, Error, TEXT("Failed to send WAV file. Response Code: %d"), Res.IsValid() ? Res->GetResponseCode() : -1);
			}
		});

	// 요청 실행
	Request->ProcessRequest();
}

void AVoiceCharacter::SendWavFileDirectly()
{


	// Wav 파일 전송 로직
	UE_LOG(LogTemp, Log, TEXT("Wav 파일 전송 시작!"));
	
	FString FilePath = TEXT("C:/AIPW/Sonheim/Saved/BouncedWavFiles/Test.wav");
	TArray<uint8> BinaryData;
	LoadWavFileBinary(FilePath, BinaryData);

	if (BinaryData.Num() > 0)
	{
		SendWavFileAsFormData(BinaryData);
	}
}








