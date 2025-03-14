// Fill out your copyright notice in the Description page of Project Settings.


#include "SonheimPlayer.h"
#include "SonheimPlayerController.h"
#include "SonheimPlayerState.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "http.h"
#include "JsonObjectConverter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sonheim/Animation/Player/PlayerAniminstance.h"
#include "Sonheim/AreaObject/Attribute/LevelComponent.h"
#include "Sonheim/AreaObject/Skill/Base/BaseSkill.h"
#include "Sonheim/AreaObject/Utility/GhostTrail.h"
#include "Sonheim/Utilities/LogMacro.h"
#include "Sonheim/UI/Widget/Player/PlayerStatusWidget.h"


class UEnhancedInputLocalPlayerSubsystem;
// Sets default values
ASonheimPlayer::ASonheimPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set AreaObject ID
	m_AreaObjectID = 1;

	// Die Setting
	DestroyDelayTime = 3.0f;

	// Set Size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(30.f, 96.f);

	// Set Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempSkeletalMesh(
		TEXT("/Script/Engine.SkeletalMesh'/Game/_Resource/Kazan/SM_Kazan.SM_Kazan'"));
	if (tempSkeletalMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempSkeletalMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -97.f), FRotator(0, -90, 0));
		GetMesh()->SetRelativeScale3D(FVector(0.4f));
	}
	
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponComponent->SetupAttachment(GetMesh(),TEXT("Weapon_R"));

	// Set Animation Blueprint
	ConstructorHelpers::FClassFinder<UAnimInstance> TempABP(TEXT(
		"/Script/Engine.AnimBlueprint'/Game/_BluePrints/AreaObject/Player/ABP_Player_Kazan_AnimInstance.ABP_Player_Kazan_AnimInstance_C'"));

	if (TempABP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(TempABP.Class);
	}

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	// Rotation Setting
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // at this rotation rate

	// Movement Setting
	GetCharacterMovement()->MaxWalkSpeed = MAX_WALK_SPEED;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;

	// Create Camera Boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocation({0, 0, 40});

	CameraBoom->TargetArmLength = 300.0f; // The Camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	// Camera Lagging
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 10.0f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->FieldOfView = 100;

	//
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
}

// Called when the game starts or when spawned
void ASonheimPlayer::BeginPlay()
{
	Super::BeginPlay();

	S_PlayerAnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	S_PlayerController = Cast<ASonheimPlayerController>(GetController());
	S_PlayerState = Cast<ASonheimPlayerState>(GetPlayerState());

	S_PlayerController->InitializeHUD();

	InitializeStateRestrictions();

	// 게임 시작 시 첫 위치를 체크포인트로 저장
	SaveCheckpoint(GetActorLocation(), GetActorRotation());
}

void ASonheimPlayer::OnDie()
{
	Super::OnDie();
	SetPlayerState(EPlayerState::DIE);
	S_PlayerController->FailWidget->AddToViewport();
	S_PlayerController->GetPlayerStatusWidget()->SetVisibility(ESlateVisibility::Hidden);
	// ToDo : TimerHandle 정리?

	//GetCharacterMovement()->SetMovementMode(MOVE_None);
}

void ASonheimPlayer::OnRevival()
{
	Super::OnRevival();
	S_PlayerController->FailWidget->RemoveFromParent();
	S_PlayerController->GetPlayerStatusWidget()->SetVisibility(ESlateVisibility::Visible);
}

void ASonheimPlayer::Reward(int ItemID, int ItemValue) const
{
	S_PlayerState->AddItem(ItemID, ItemValue);
	m_LevelComponent->AddExp(10);
}

// Called every frame
void ASonheimPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// T키를 눌렀을 때 WAV 파일 전송 테스트
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::T))
	{
		SendWavFileDirectly();
	}
	
}

// WAV 파일을 로드하고 바이너리 데이터로 전환
void ASonheimPlayer::LoadWavFileBinary(const FString& FilePath, TArray<uint8>& BinaryData)
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


TArray<uint8> ASonheimPlayer::FStringToUint8(FString str)
{
	TArray<uint8> outBytes;

	FTCHARToUTF8 converted(*str);
	outBytes.Append(reinterpret_cast<const uint8*>(converted.Get()), converted.Length());

	return outBytes;
}


// WAV 파일을 multipart/form-data 형식으로 전송
void ASonheimPlayer::SendWavFileAsFormData(const TArray<uint8>& BinaryData)
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
				UE_LOG(LogTemp, Log, TEXT("Successfully sent WAV file! 1: %hhd"), InfoArray.actor);
				UE_LOG(LogTemp, Log, TEXT("Successfully sent WAV file! 2: %hhd"), InfoArray.forced);
				UE_LOG(LogTemp, Log, TEXT("Successfully sent WAV file! 3: %hhd"), InfoArray.target);
				UE_LOG(LogTemp, Log, TEXT("Successfully sent WAV file! 4: %hhd"), InfoArray.work);
				
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

void ASonheimPlayer::SendWavFileDirectly()
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


void ASonheimPlayer::InitializeStateRestrictions()
{
	// 일반 상태 - 모든 행동 가능
	FActionRestrictions NormalRestrictions;
	StateRestrictions.Add(EPlayerState::NORMAL, NormalRestrictions);

	// Only Rotate 상태 - 회전만 가능
	FActionRestrictions OnlyRotateRestrictions;
	OnlyRotateRestrictions.bCanMove = false;
	OnlyRotateRestrictions.bCanOnlyRotate = true;
	OnlyRotateRestrictions.bCanAction = false;
	StateRestrictions.Add(EPlayerState::ONLY_ROTATE, OnlyRotateRestrictions);

	// Action 상태 - 이동, Action 제한
	FActionRestrictions ActionRestrictions;
	ActionRestrictions.bCanMove = false;
	ActionRestrictions.bCanAction = false;
	StateRestrictions.Add(EPlayerState::ACTION, ActionRestrictions);

	// Can Action 상태 - 이동, 제한
	FActionRestrictions CanActionRestrictions;
	CanActionRestrictions.bCanMove = false;
	StateRestrictions.Add(EPlayerState::CANACTION, CanActionRestrictions);

	// Die - 삭제할수도?
	FActionRestrictions DieRestrictions;
	DieRestrictions.bCanMove = false;
	DieRestrictions.bCanAction = false;
	StateRestrictions.Add(EPlayerState::DIE, DieRestrictions);

	SetPlayerState(EPlayerState::NORMAL);
}

bool ASonheimPlayer::CanPerformAction(EPlayerState State, FString ActionName)
{
	if (!StateRestrictions.Contains(State))
		return false;

	const FActionRestrictions& Restrictions = StateRestrictions[State];

	if (ActionName == "Move")
		return Restrictions.bCanMove;
	else if (ActionName == "Rotate")
		return Restrictions.bCanRotate;
	else if (ActionName == "OnlyRotate")
		return Restrictions.bCanOnlyRotate;
	else if (ActionName == "Look")
		return Restrictions.bCanLook;
	else if (ActionName == "Action")
		return Restrictions.bCanAction;

	return false;
}



void ASonheimPlayer::SetComboState(bool bCanCombo, int SkillID)
{
	CanCombo = bCanCombo;
	NextComboSkillID = SkillID;
}

void ASonheimPlayer::SetPlayerState(EPlayerState NewState)
{
	CurrentPlayerState = NewState;

	// 상태 변경에 따른 추가 처리
	const FActionRestrictions& NewRestrictions = StateRestrictions[NewState];

	// 이동 제한 적용 - Root Motion도 제한하므로 생각해야할듯..
	//if (!NewRestrictions.bCanMove)
	//	GetCharacterMovement()->DisableMovement();
	//else
	//	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// 회전 제한 적용
	GetCharacterMovement()->bOrientRotationToMovement = NewRestrictions.bCanRotate;
}

void ASonheimPlayer::Move(const FVector2D MovementVector)
{
	// input is a Vector2D
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add Movement
		if (CanPerformAction(CurrentPlayerState, "Move"))
		{
			S_PlayerAnimInstance->Montage_Stop(0.2f);
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}
		// Rotate Only
		if (CanPerformAction(CurrentPlayerState, "OnlyRotate"))
		{
			FVector targetLocation = GetActorLocation() + RightDirection * MovementVector.X + ForwardDirection *
				MovementVector.Y;

			LookAtLocation(targetLocation, EPMRotationMode::Speed, 1000.f);
		}
	}
}

void ASonheimPlayer::Look(const FVector2D LookAxisVector)
{
	// input is a Vector2D
	if (Controller != nullptr && CanPerformAction(CurrentPlayerState, "Look"))
	{
		// add yaw and pitch input to controller
		// 상하 회전 제한 적용

		//float oldPitchAngle = GetControlRotation().Pitch;
		//float newPitchAngle = oldPitchAngle + (LookAxisVector.Y * LookSensitivityY);
		//newPitchAngle = FMath::ClampAngle(newPitchAngle, MinPitchAngle, MaxPitchAngle);
		//float pitchInput = newPitchAngle - oldPitchAngle;
		float newPitchAngle = CurrentPitchAngle + (LookAxisVector.Y * LookSensitivityY);

		//newPitchAngle = FMath::ClampAngle(newPitchAngle, MinPitchAngle, MaxPitchAngle);
		float pitchInput = newPitchAngle - CurrentPitchAngle;

		// 좌우 회전
		AddControllerYawInput(LookAxisVector.X * LookSensitivityX);
		// 상하 회전
		AddControllerPitchInput(pitchInput);

		CurrentPitchAngle = newPitchAngle;
	}
}

void ASonheimPlayer::LeftMouse_Pressed()
{
	if (!CanPerformAction(CurrentPlayerState, "Action")) return;

	if (CanCombo && NextComboSkillID)
	{
		TObjectPtr<UBaseSkill> comboSkill = GetSkillByID(NextComboSkillID);
		if (CastSkill(comboSkill, this))
		{
			SetPlayerState(EPlayerState::ACTION);
			comboSkill->OnSkillComplete.BindUObject(this, &ASonheimPlayer::SetPlayerNormalState);
		}
	}

	int weakAttackID = 10;
	TObjectPtr<UBaseSkill> skill = GetSkillByID(weakAttackID);
	if (CastSkill(skill, this))
	{
		SetPlayerState(EPlayerState::ACTION);
		skill->OnSkillComplete.BindUObject(this, &ASonheimPlayer::SetPlayerNormalState);
		//skill->OnSkillCancel.BindUObject(this, &APlayer_Kazan::SetPlayerNormalState);
	}
}

void ASonheimPlayer::RightMouse_Pressed()
{
}

void ASonheimPlayer::Dodge_Pressed()
{
	if (!CanPerformAction(CurrentPlayerState, "Action")) return;
	int dodgeSkillID = 2;

	TObjectPtr<UBaseSkill> skill = GetSkillByID(dodgeSkillID);
	if (CastSkill(skill, this))
	{
		SetPlayerState(EPlayerState::ACTION);
		skill->OnSkillComplete.BindUObject(this, &ASonheimPlayer::SetPlayerNormalState);
	}
}

void ASonheimPlayer::Jump_Pressed()
{
	Jump();
}

void ASonheimPlayer::Jump_Released()
{
	StopJumping();
}


void ASonheimPlayer::Restart_Pressed()
{
	if (!IsDie())
	{
		return;
	}
	// 
	RespawnAtCheckpoint();
}

void ASonheimPlayer::SaveCheckpoint(FVector Location, FRotator Rotation)
{
	LastCheckpointLocation = Location;
	LastCheckpointRotation = Rotation;
}

void ASonheimPlayer::RespawnAtCheckpoint()
{
	OnRevival();
	// 캐릭터 위치 및 회전 설정
	SetActorLocation(LastCheckpointLocation);
	SetActorRotation(LastCheckpointRotation);

	// ToDo : 리스폰 초기화
	SetPlayerState(EPlayerState::NORMAL);
}
