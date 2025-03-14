// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VoiceCharacter.generated.h"

UCLASS()
class SONHEIM_API AVoiceCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVoiceCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


// 	// WAV 파일을 로드하고 바이너리 데이터로 전환
// 	void LoadWavFileBinary(const FString& FilePath, TArray<uint8>& BinaryData);
//
// 	// WAV 파일을 multipart/form-data 형식으로 전송
// 	void SendWavFileAsFormData(const TArray<uint8>& BinaryData);
//
// 	// WAV 파일을 직접 전송하는 함수
// 	void SendWavFileDirectly();
//
// private:
// 	TArray<uint8> FStringToUint8(FString str);
};

