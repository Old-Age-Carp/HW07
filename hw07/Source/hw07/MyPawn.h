// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class HW07_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	// Allows a Pawn to set up custom input bindings -> 내가 원하는 input을 bind하기 위해선 SetupPlayerInputComponent사용
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// 캡슐 메시 스프링암 카메라 컴포넌트 선언
	// TObjectPtr -> Smart Ponter -> 타입 검사가 엄격 and 메모리 관리가 쉽습니다.
	// TObjectPtr<UCapsuleComponent> CapsuleComp -> CapsuleComp는 UCapsuleComponent 타입(자식)의 주소만 받는다!
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "components")
	TObjectPtr<UCapsuleComponent> CapsuleComp; // 충돌 처리

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "components")
	TObjectPtr<USkeletalMeshComponent> Mesh; // 외형 표시

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "components")
	TObjectPtr<USpringArmComponent> SpringArm; // 3인칭 하기 위한 셀카봉

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "components")
	TObjectPtr<UCameraComponent> Camera; // 화면찍는 카메라

	// Enhanced Input 관련 변수들
	// UInputAction -> 플레이어 행동 -> MoveAction : 이동 , LookAction : 보기 -> 행동에 이름을 붙임.
	// UInputMappingContext -> w키가 눌렸다면 앞으로(x축) +1로 한다 -> Input을 Mapping 한다
	// SetupPlayerInputComponent -> MoveAction 이 발동 -> Move() 호출
	// SetupPlayerInputComponent -> LookAction 이 발동 -> Look() 호출
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	// 이동, 회전 속도 관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MoveSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float LookSpeed = 100.f;

	// 도전 과제
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	//TObjectPtr<UInputAction> AscendAction; // 상/하 이동 액션

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	//TObjectPtr<UInputAction> RollAction; // 좌/우 기울기(roll)용 액션
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movemetn|6DOF") // Six Degrees of Freedom
	//float AscendSpeed = 300.f; // 상/하 속도

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movemetn|6DOF")
	//float RollSpeed = 100.f; // 좌/우 기울기 속도

private:
	// 입력 처리 함수
	void Move(const FInputActionValue& Value);
	void StopMove(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	// 입력 값을 저장할 변수들
	FVector2D CurrentMoveInput;

};