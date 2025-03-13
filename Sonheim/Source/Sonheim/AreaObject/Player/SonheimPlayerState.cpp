// Fill out your copyright notice in the Description page of Project Settings.


#include "SonheimPlayerState.h"

#include "SonheimPlayer.h"
#include "Sonheim/GameManager/SonheimGameInstance.h"
#include "Sonheim/Utilities/LogMacro.h"

void ASonheimPlayerState::BeginPlay()
{
	Super::BeginPlay();

	m_GameInstance = Cast<USonheimGameInstance>(GetGameInstance());
	m_Player = Cast<ASonheimPlayer>(GetOwner());
	
	// 장비 슬롯 초기화
	for (int i = 0; i < static_cast<int>(EEquipmentSlotType::Accessory2) + 1; i++)
	{
		EEquipmentSlotType SlotType = static_cast<EEquipmentSlotType>(i);
		if (SlotType != EEquipmentSlotType::None)
		{
			EquippedItems.Add(SlotType, 0); // 0은 비어있음을 의미
		}
	}
}

bool ASonheimPlayerState::AddItem(int ItemID, int ItemCount)
{
	if (ItemCount <= 0 || !IsValidItemID(ItemID))
		return false;
		
	FItemData* ItemData = GetItemData(ItemID);
	if (!ItemData)
		return false;
		
	// 인벤토리에서 같은 아이템 찾기
	int ExistingItemIndex = FindItemIndexInInventory(ItemID);
	
	if (ExistingItemIndex != INDEX_NONE)
	{
		// 이미 있는 아이템, 수량 증가
		InventoryItems[ExistingItemIndex].Count += ItemCount;
	}
	else
	{
		// 새 아이템 추가
		if (InventoryItems.Num() >= MaxInventorySlots)
			return false; // 인벤토리 가득 참
			
		InventoryItems.Add(FInventoryItem(ItemID, ItemCount));
	}
	
	// 이벤트 발생
	OnItemAdded.Broadcast(ItemID, ItemCount);
	BroadcastInventoryChanged();
	
	return true;
}

bool ASonheimPlayerState::RemoveItem(int ItemID, int ItemCount)
{
	if (ItemCount <= 0 || !IsValidItemID(ItemID))
		return false;
		
	int ItemIndex = FindItemIndexInInventory(ItemID);
	if (ItemIndex == INDEX_NONE)
		return false; // 아이템 없음
		
	FInventoryItem& Item = InventoryItems[ItemIndex];
	
	if (Item.Count < ItemCount)
		return false; // 충분한 수량 없음
		
	// 수량 감소
	Item.Count -= ItemCount;
	
	// 수량이 0이면 제거
	if (Item.Count <= 0)
	{
		// 장착된 아이템이면 장착 해제
		if (Item.bIsEquipped)
		{
			FItemData* ItemData = GetItemData(ItemID);
			if (ItemData && ItemData->EquipSlot != EEquipmentSlotType::None)
			{
				UnequipItem(ItemData->EquipSlot);
			}
		}
		
		InventoryItems.RemoveAt(ItemIndex);
	}
	
	// 이벤트 발생
	OnItemRemoved.Broadcast(ItemID, ItemCount);
	BroadcastInventoryChanged();
	
	return true;
}

bool ASonheimPlayerState::EquipItem(int ItemID)
{
	if (!IsValidItemID(ItemID))
		return false;
		
	FItemData* ItemData = GetItemData(ItemID);
	if (!ItemData || ItemData->EquipSlot == EEquipmentSlotType::None)
		return false; // 장착 불가능한 아이템
		
	// 인벤토리에서 아이템 찾기
	int ItemIndex = FindItemIndexInInventory(ItemID);
	if (ItemIndex == INDEX_NONE)
		return false; // 아이템 없음
		
	// 같은 슬롯에 장착된 아이템이 있으면 해제
	if (EquippedItems.Contains(ItemData->EquipSlot) && EquippedItems[ItemData->EquipSlot] != 0)
	{
		UnequipItem(ItemData->EquipSlot);
	}
	
	// 아이템 장착
	EquippedItems[ItemData->EquipSlot] = ItemID;
	InventoryItems[ItemIndex].bIsEquipped = true;
	
	// 스탯 적용
	ApplyEquipmentStats(ItemID, true);
	
	// 이벤트 발생
	OnEquipmentChanged.Broadcast(ItemData->EquipSlot, ItemID);
	BroadcastInventoryChanged();
	
	return true;
}

bool ASonheimPlayerState::UnequipItem(EEquipmentSlotType SlotType)
{
	if (SlotType == EEquipmentSlotType::None || !EquippedItems.Contains(SlotType))
		return false;
		
	int ItemID = EquippedItems[SlotType];
	if (ItemID == 0)
		return false; // 슬롯이 비어있음
		
	// 인벤토리에서 아이템 찾기
	int ItemIndex = FindItemIndexInInventory(ItemID);
	if (ItemIndex != INDEX_NONE)
	{
		InventoryItems[ItemIndex].bIsEquipped = false;
	}
	
	// 스탯 제거
	ApplyEquipmentStats(ItemID, false);
	
	// 장착 해제
	EquippedItems[SlotType] = 0;
	
	// 이벤트 발생
	OnEquipmentChanged.Broadcast(SlotType, 0);
	BroadcastInventoryChanged();
	
	return true;
}

bool ASonheimPlayerState::HasItem(int ItemID, int RequiredCount) const
{
	if (RequiredCount <= 0 || !IsValidItemID(ItemID))
		return false;
		
	int ItemIndex = FindItemIndexInInventory(ItemID);
	if (ItemIndex == INDEX_NONE)
		return false;
		
	return InventoryItems[ItemIndex].Count >= RequiredCount;
}

int ASonheimPlayerState::GetItemCount(int ItemID) const
{
	int ItemIndex = FindItemIndexInInventory(ItemID);
	if (ItemIndex == INDEX_NONE)
		return 0;
		
	return InventoryItems[ItemIndex].Count;
}

TArray<FInventoryItem> ASonheimPlayerState::GetInventory() const
{
	return InventoryItems;
}

TMap<EEquipmentSlotType, int> ASonheimPlayerState::GetEquippedItems() const
{
	return EquippedItems;
}

bool ASonheimPlayerState::IsValidItemID(int ItemID) const
{
	return m_GameInstance && m_GameInstance->GetDataItem(ItemID) != nullptr;
}

FItemData* ASonheimPlayerState::GetItemData(int ItemID) const
{
	return m_GameInstance ? m_GameInstance->GetDataItem(ItemID) : nullptr;
}

void ASonheimPlayerState::ApplyEquipmentStats(int ItemID, bool bEquipping)
{
	FItemData* ItemData = GetItemData(ItemID);
	if (!ItemData || !m_Player)
		return;
		
	float Multiplier = bEquipping ? 1.0f : -1.0f;
	
	// 스탯 적용 (실제 구현은 플레이어 클래스에 맞게 조정 필요)
	if (ItemData->HPBonus != 0)
	{
		// 최대 체력 조정
		// m_Player->UpdateMaxHP(ItemData->HPBonus * Multiplier);
	}
	
	if (ItemData->StaminaBonus != 0)
	{
		// 최대 스태미나 조정
		// m_Player->UpdateMaxStamina(ItemData->StaminaBonus * Multiplier);
	}
}

int ASonheimPlayerState::FindItemIndexInInventory(int ItemID) const
{
	for (int i = 0; i < InventoryItems.Num(); i++)
	{
		if (InventoryItems[i].ItemID == ItemID)
			return i;
	}
	
	return INDEX_NONE;
}

void ASonheimPlayerState::BroadcastInventoryChanged()
{
	OnInventoryChanged.Broadcast(InventoryItems);
}
