// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Sonheim/ResourceManager/SonheimGameType.h"
#include "SonheimPlayerState.generated.h"

class ASonheimPlayer;
class USonheimGameInstance;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryChanged, const TArray<FInventoryItem>&, Inventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipmentChanged, EEquipmentSlotType, Slot, int, ItemID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAdded, int, ItemID, int, Count);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemoved, int, ItemID, int, Count);

UCLASS()
class SONHEIM_API ASonheimPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// 캐릭터 스탯
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stats")
	int32 Level = 1;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stats")
	int32 Strength = 10;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stats")
	int32 Dexterity = 10;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stats")
	int32 Vitality = 10;
    
	// 인벤토리 시스템
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	int32 MaxInventorySlots = 50;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TArray<FInventoryItem> InventoryItems;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TMap<EEquipmentSlotType, int> EquippedItems;
    
public:
	// 인벤토리 관련
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItem(int ItemID, int ItemCount);
    
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItem(int ItemID, int ItemCount);
    
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool EquipItem(int ItemID);
    
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool UnequipItem(EEquipmentSlotType SlotType);
    
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool HasItem(int ItemID, int RequiredCount = 1) const;
    
	UFUNCTION(BlueprintCallable, Category="Inventory")
	int GetItemCount(int ItemID) const;
    
	UFUNCTION(BlueprintCallable, Category="Inventory")
	TArray<FInventoryItem> GetInventory() const;
    
	UFUNCTION(BlueprintCallable, Category="Inventory")
	TMap<EEquipmentSlotType, int> GetEquippedItems() const;

	// 이벤트 델리게이트
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnInventoryChanged OnInventoryChanged;
    
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnEquipmentChanged OnEquipmentChanged;
    
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnItemAdded OnItemAdded;
    
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnItemRemoved OnItemRemoved;

private:
	UPROPERTY()
	USonheimGameInstance* m_GameInstance;
	UPROPERTY()
	ASonheimPlayer* m_Player;
    
	// 내부 헬퍼 함수
	bool IsValidItemID(int ItemID) const;
	struct FItemData* GetItemData(int ItemID) const;
	void ApplyEquipmentStats(int ItemID, bool bEquipping);
	int FindItemIndexInInventory(int ItemID) const;
	void BroadcastInventoryChanged();
};
