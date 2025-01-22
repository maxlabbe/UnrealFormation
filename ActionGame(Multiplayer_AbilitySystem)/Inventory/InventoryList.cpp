// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryList.h"
#include "Inventory/InventoryItemInstance.h"
#include "ActionGameTypes.h"

void FInventoryList::AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	FInventoryListItem& Item = Items.AddDefaulted_GetRef();
	UInventoryItemInstance* ItemInstance = NewObject<UInventoryItemInstance>();
	ItemInstance->Init(InItemStaticDataClass);
	Item.ItemInstance = ItemInstance;
	MarkItemDirty(Item);
}

void FInventoryList::AddItem(UInventoryItemInstance* InItemInstance)
{
	FInventoryListItem& Item = Items.AddDefaulted_GetRef();
	Item.ItemInstance = InItemInstance;
	MarkItemDirty(Item);
}

void FInventoryList::RemoveItem(TSubclassOf<UItemStaticData> InItemDataStaticClass)
{
	for (auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
	{
		FInventoryListItem Item = *ItemIter;
		if (Item.ItemInstance && Item.ItemInstance->GetItemStaticData()->IsA(InItemDataStaticClass))
		{
			ItemIter.RemoveCurrent();
			MarkArrayDirty();
			break;
		}
	}
}
