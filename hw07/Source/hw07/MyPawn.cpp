// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 캡슐 컴포넌트를 생성하고 루트로 설정
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	RootComponent = CapsuleComp;
	//요구사항: 물리 시뮬레이션 off
	CapsuleComp->SetSimulatePhysics(false);

	//스켈레탈 메시 컴포넌트를 생성하고 캡슐에 부착
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	//요구사항 -> 물리 시뮬레이션  off
	Mesh->SetSimulatePhysics(false);

	//스프링 암을 생성하고 캡슐에 부착 (셀카봉)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f; //카메라와의 거리

	// 카메라를 생성하고 스프링 암 끝에 부착
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	
	//게임 시작 시 Enhanced Input 시스템을 활성화한다.
	// 현재 pawn controller -> APlayerController 로 변환
	// PlayerController -> Enhanced Input Local Player Subsystem을 가져옵니다.
	// 마지막으로 가져온 Mapping Context 추가
	// 블루프린트에서 만든 키 설정(IMC)을 실제 게임에서 인식하도록 시스템에 등록
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//이동 구현
	// 앞/뒤 이동 x축
	// 와/우 이동 y축
	FVector DeltaLocation(CurrentMoveInput.X, CurrentMoveInput.Y, 0.f);

	// 대각선 이동이 빨라지는것을 막는 벡터 정규화(Normalize)
	//직각 삼각형 생각해보면 -> 밑변, 높이가 1, 1이면 대각선은 root2(1.414..)입니다.
	DeltaLocation.Normalize();

	// 최종 이동량 계산 및 적용
	AddActorLocalOffset(DeltaLocation * MoveSpeed * DeltaTime);
}

void AMyPawn::Move(const FInputActionValue& Value)
{
	// WASD 입력 값을 변수에 저장
	// 이동은 Tick에서 처리
	CurrentMoveInput = Value.Get<FVector2D>(); // (1,0)
}

void AMyPawn::StopMove(const FInputActionValue& Value)
{
	CurrentMoveInput = FVector2D::ZeroVector; // (0,0)
}

void AMyPawn::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>(); // (1,0)

	// AddControllerInput 대신 직접 Actor 회전
	// 좌/우 회전(Yaw)은 Pawn 전체를 직접 회전
	// GetWorld()->GetDeltaSeconds() -> Tick의 DeltaTime과 같은 값
	AddActorLocalRotation(FRotator(0.f, LookAxisVector.X * LookSpeed * GetWorld()->GetDeltaSeconds(), 0.f));

	// 위/아래 회전(pitch)은 카메라가 달려있는 스프링 암을 회전 -> 캐릭터가 앞/뒤로 움직이면 안됩니다.
	FRotator CurrentSpringArmRotation = SpringArm->GetRelativeRotation(); // 현재 SpringArm의 rotation 가져오기
	// 현재 pitch에서 - 마우스의 위 아래 * 회전 속도 * 프레임 간격
	float NewPitch = CurrentSpringArmRotation.Pitch - LookAxisVector.Y * LookSpeed * GetWorld()->GetDeltaSeconds();

	// 카메라가 땅을 파고들거나 하늘만 보지 않도록 각도를 제한
	NewPitch = FMath::Clamp(NewPitch, -75.f, 0.f);

	SpringArm->SetRelativeRotation(FRotator(NewPitch, 0.f, 0.f));
}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// PlayerInputComponent -> EnhancedInputComponent로 캐스팅
	// // 예전 Input 시스템 안쓰고 최신꺼 쓰겠다.
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// IA_Move 액션이 발동하면 이 클래스의 Move 함수를 호출하도록 바인딩
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPawn::Move); // MoveAction 이 / 발동되면 / 현재 객체 / Move함수 실행
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AMyPawn::StopMove); // Completed -> StopMove 실행
		// IA_Look 액션도 똑같이 바인딩 합니다
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPawn::Look);
	}
}

