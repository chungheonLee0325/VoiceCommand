
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sonheim/ResourceManager/SonheimGameType.h"
#include "EquipmentComponent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipmentChanged, EEquipmentSlotType, SlotType, int32, ItemID);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SONHEIM_API UEquipmentComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEquipmentComponent();
/*
    // 장비 아이템 착용
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    bool EquipItem(int32 ItemID, EEquipmentSlotType SlotType);

    // 장비 해제
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    bool UnequipItem(EEquipmentSlotType SlotType);

    // 현재 장착된 아이템 ID 반환
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    int32 GetEquippedItemInSlot(EEquipmentSlotType SlotType) const;

    // 장비 변경 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Equipment")
    FOnEquipmentChanged OnEquipmentChanged;

    // 특정 슬롯에 장비가 있는지 확인
    UFUNCTION(BlueprintPure, Category = "Equipment")
    bool HasEquippedItem(EEquipmentSlotType SlotType) const;

    // 현재 주무기 타입 확인
    UFUNCTION(BlueprintPure, Category = "Equipment")
    EWeaponType GetCurrentWeaponType() const;

protected:
    virtual void BeginPlay() override;

    // 장비 슬롯별 아이템 ID 저장
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
    TMap<EEquipmentSlotType, int32> EquippedItems;

    // 장비 메시 업데이트
    void UpdateEquipmentMesh(EEquipmentSlotType SlotType, int32 ItemID);

    // 장비 능력 및 동작 업데이트
    void UpdateEquipmentCapabilities(EEquipmentSlotType SlotType, int32 NewItemID, int32 OldItemID);

private:
    UPROPERTY()
    class ASonheimPlayer* OwnerPlayer;
    
    UPROPERTY()
    class USonheimGameInstance* GameInstance;
    */
};

inline UEquipmentComponent::UEquipmentComponent()
{
}
