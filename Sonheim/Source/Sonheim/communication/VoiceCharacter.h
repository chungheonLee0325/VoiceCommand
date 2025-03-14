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

	// WAV 파일을 서버로 전송하는 함수
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SendWavFileDirectly();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void LoadWavFileBinary(const FString& FilePath, TArray<uint8>& BinaryData); // WAV 파일 로드
	void SendBinaryDataToServer(const TArray<uint8>& BinaryData); // 바이너리 데이터를 서버로 전송
};

