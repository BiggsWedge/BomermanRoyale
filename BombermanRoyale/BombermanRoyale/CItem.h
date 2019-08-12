#pragma once
#include "CObject.h"
class CItem :
	public CObject
{
private:
	int itemType;
public:
	CItem();
	~CItem();

	inline int GetItemType() { return itemType; }
	inline void SetItemType(int _itemType) { itemType = _itemType; }
};

