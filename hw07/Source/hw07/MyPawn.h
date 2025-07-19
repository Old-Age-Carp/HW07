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
	// Allows a Pawn to set up custom input bindings -> ���� ���ϴ� input�� bind�ϱ� ���ؼ� SetupPlayerInputComponent���
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// ĸ�� �޽� �������� ī�޶� ������Ʈ ����
	// TObjectPtr -> Smart Ponter -> Ÿ�� �˻簡 ���� and �޸� ������ �����ϴ�.
	// TObjectPtr<UCapsuleComponent> CapsuleComp -> CapsuleComp�� UCapsuleComponent Ÿ��(�ڽ�)�� �ּҸ� �޴´�!
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "components")
	TObjectPtr<UCapsuleComponent> CapsuleComp; // �浹 ó��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "components")
	TObjectPtr<USkeletalMeshComponent> Mesh; // ���� ǥ��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "components")
	TObjectPtr<USpringArmComponent> SpringArm; // 3��Ī �ϱ� ���� ��ī��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "components")
	TObjectPtr<UCameraComponent> Camera; // ȭ����� ī�޶�

	// Enhanced Input ���� ������
	// UInputAction -> �÷��̾� �ൿ -> MoveAction : �̵� , LookAction : ���� -> �ൿ�� �̸��� ����.
	// UInputMappingContext -> wŰ�� ���ȴٸ� ������(x��) +1�� �Ѵ� -> Input�� Mapping �Ѵ�
	// SetupPlayerInputComponent -> MoveAction �� �ߵ� -> Move() ȣ��
	// SetupPlayerInputComponent -> LookAction �� �ߵ� -> Look() ȣ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	// �̵�, ȸ�� �ӵ� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MoveSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float LookSpeed = 100.f;

	// ���� ����
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	//TObjectPtr<UInputAction> AscendAction; // ��/�� �̵� �׼�

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	//TObjectPtr<UInputAction> RollAction; // ��/�� ����(roll)�� �׼�
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movemetn|6DOF") // Six Degrees of Freedom
	//float AscendSpeed = 300.f; // ��/�� �ӵ�

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movemetn|6DOF")
	//float RollSpeed = 100.f; // ��/�� ���� �ӵ�

private:
	// �Է� ó�� �Լ�
	void Move(const FInputActionValue& Value);
	void StopMove(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	// �Է� ���� ������ ������
	FVector2D CurrentMoveInput;

};