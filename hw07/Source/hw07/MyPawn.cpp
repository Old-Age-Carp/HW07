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

	// ĸ�� ������Ʈ�� �����ϰ� ��Ʈ�� ����
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	RootComponent = CapsuleComp;
	//�䱸����: ���� �ùķ��̼� off
	CapsuleComp->SetSimulatePhysics(false);

	//���̷�Ż �޽� ������Ʈ�� �����ϰ� ĸ���� ����
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	//�䱸���� -> ���� �ùķ��̼�  off
	Mesh->SetSimulatePhysics(false);

	//������ ���� �����ϰ� ĸ���� ���� (��ī��)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f; //ī�޶���� �Ÿ�

	// ī�޶� �����ϰ� ������ �� ���� ����
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	
	//���� ���� �� Enhanced Input �ý����� Ȱ��ȭ�Ѵ�.
	// ���� pawn controller -> APlayerController �� ��ȯ
	// PlayerController -> Enhanced Input Local Player Subsystem�� �����ɴϴ�.
	// ���������� ������ Mapping Context �߰�
	// �������Ʈ���� ���� Ű ����(IMC)�� ���� ���ӿ��� �ν��ϵ��� �ý��ۿ� ���
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

	//�̵� ����
	// ��/�� �̵� x��
	// ��/�� �̵� y��
	FVector DeltaLocation(CurrentMoveInput.X, CurrentMoveInput.Y, 0.f);

	// �밢�� �̵��� �������°��� ���� ���� ����ȭ(Normalize)
	//���� �ﰢ�� �����غ��� -> �غ�, ���̰� 1, 1�̸� �밢���� root2(1.414..)�Դϴ�.
	DeltaLocation.Normalize();

	// ���� �̵��� ��� �� ����
	AddActorLocalOffset(DeltaLocation * MoveSpeed * DeltaTime);
}

void AMyPawn::Move(const FInputActionValue& Value)
{
	// WASD �Է� ���� ������ ����
	// �̵��� Tick���� ó��
	CurrentMoveInput = Value.Get<FVector2D>(); // (1,0)
}

void AMyPawn::StopMove(const FInputActionValue& Value)
{
	CurrentMoveInput = FVector2D::ZeroVector; // (0,0)
}

void AMyPawn::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>(); // (1,0)

	// AddControllerInput ��� ���� Actor ȸ��
	// ��/�� ȸ��(Yaw)�� Pawn ��ü�� ���� ȸ��
	// GetWorld()->GetDeltaSeconds() -> Tick�� DeltaTime�� ���� ��
	AddActorLocalRotation(FRotator(0.f, LookAxisVector.X * LookSpeed * GetWorld()->GetDeltaSeconds(), 0.f));

	// ��/�Ʒ� ȸ��(pitch)�� ī�޶� �޷��ִ� ������ ���� ȸ�� -> ĳ���Ͱ� ��/�ڷ� �����̸� �ȵ˴ϴ�.
	FRotator CurrentSpringArmRotation = SpringArm->GetRelativeRotation(); // ���� SpringArm�� rotation ��������
	// ���� pitch���� - ���콺�� �� �Ʒ� * ȸ�� �ӵ� * ������ ����
	float NewPitch = CurrentSpringArmRotation.Pitch - LookAxisVector.Y * LookSpeed * GetWorld()->GetDeltaSeconds();

	// ī�޶� ���� �İ��ų� �ϴø� ���� �ʵ��� ������ ����
	NewPitch = FMath::Clamp(NewPitch, -75.f, 0.f);

	SpringArm->SetRelativeRotation(FRotator(NewPitch, 0.f, 0.f));
}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// PlayerInputComponent -> EnhancedInputComponent�� ĳ����
	// // ���� Input �ý��� �Ⱦ��� �ֽŲ� ���ڴ�.
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// IA_Move �׼��� �ߵ��ϸ� �� Ŭ������ Move �Լ��� ȣ���ϵ��� ���ε�
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPawn::Move); // MoveAction �� / �ߵ��Ǹ� / ���� ��ü / Move�Լ� ����
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AMyPawn::StopMove); // Completed -> StopMove ����
		// IA_Look �׼ǵ� �Ȱ��� ���ε� �մϴ�
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPawn::Look);
	}
}

