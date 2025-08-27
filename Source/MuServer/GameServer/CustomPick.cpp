// CustomPick.cpp: implementation of the CCustomPick class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomPick.h"
#include "CommandManager.h"
#include "Log.h"
#include "Map.h"
#include "MapManager.h"
#include "Message.h"
#include "Notice.h"
#include "ServerInfo.h"
#include "Util.h"
#include "Viewport.h"
#include "ReadScript.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomPick::CCustomPick() // OK
{
	this->m_CustomPickInfo.clear();
}

CCustomPick::~CCustomPick() // OK
{

}

void CCustomPick::Load(char* path) // OK
{
    CReadScript* lpReadScript = new CReadScript;

    if (lpReadScript == 0)
    {
        ErrorMessageBox(READ_SCRIPT_ALLOC_ERROR, path);
        return;
    }

    if (lpReadScript->Load(path) == false)
    {
        ErrorMessageBox(READ_SCRIPT_FILE_ERROR, path);
        delete lpReadScript;
        return;
    }

    this->m_CustomPickInfo.clear();

    try
    {
        eTokenResult token;

        while (true)
        {
            token = lpReadScript->GetToken();

            if (token == TOKEN_END || token == TOKEN_END_SECTION)
            {
                break;
            }

            CUSTOMPICK_INFO info;

            info.Index = lpReadScript->GetNumber();

            strcpy_s(info.Name, lpReadScript->GetAsString());

            info.Cat = lpReadScript->GetAsNumber();

            info.Item = lpReadScript->GetAsNumber();

            this->m_CustomPickInfo.insert(std::pair<int, CUSTOMPICK_INFO>(info.Index, info));
        }
    }
    catch (...)
    {
        ErrorMessageBox(lpReadScript->GetError());
    }

    delete lpReadScript;
}

bool CCustomPick::GetInfo(int index, CUSTOMPICK_INFO* lpInfo) // OK
{
	std::map<int, CUSTOMPICK_INFO>::iterator it = this->m_CustomPickInfo.find(index);

	if (it == this->m_CustomPickInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}

bool CCustomPick::GetInfoByName(char* message, CUSTOMPICK_INFO* lpInfo) // OK
{
	char command[32] = { 0 };

	memset(command, 0, sizeof(command));

	gCommandManager.GetString(message, command, sizeof(command), 0);


	for (std::map<int, CUSTOMPICK_INFO>::iterator it = this->m_CustomPickInfo.begin(); it != this->m_CustomPickInfo.end(); it++)
	{
		if (_stricmp(it->second.Name, command) == 0)
		{
			(*lpInfo) = it->second;
			return 1;
		}
	}

	return 0;
}

bool FakeisJewels(int index)
{
	if (index == GET_ITEM(12, 15) ||
		index == GET_ITEM(14, 13) ||
		index == GET_ITEM(14, 14) ||
		index == GET_ITEM(14, 16) ||
		index == GET_ITEM(14, 15) ||
		index == GET_ITEM(14, 22) ||
		index == GET_ITEM(14, 31) ||
		index == GET_ITEM(13, 14) ||
		index == GET_ITEM(14, 42))
	{
		return true;
	}

	return false;
}

void CCustomPick::ItemGet(LPOBJ lpObj) // OK
{
	if (gObjIsConnectedGP(lpObj->Index) == 0)
	{
		return;
	}

	if (lpObj->DieRegen != 0)
	{
		return;
	}

	if (lpObj->Interface.use != 0 && lpObj->Interface.type != INTERFACE_SHOP)
	{
		return;
	}

	if (lpObj->Transaction == 1)
	{
		return;
	}

	if (lpObj->Type != OBJECT_USER)
	{
		return;
	}

	int index = -1;

	for (int n = 0; n < MAX_MAP_ITEM; n++)
	{
		if (gMap[lpObj->Map].m_Item[n].m_Live == 0)
		{
			continue;
		}

		if (gMap[lpObj->Map].m_Item[n].m_State != OBJECT_CREATE && gMap[lpObj->Map].m_Item[n].m_State != OBJECT_PLAYING)
		{
			continue;
		}

		if (lpObj->PickupExc == 1 || lpObj->PickupSocket == 1 || lpObj->PickupSetItem == 1)
		{
			if (gViewport.CheckViewportObjectPosition(lpObj->Index, lpObj->Map, gMap[lpObj->Map].m_Item[n].m_X, gMap[lpObj->Map].m_Item[n].m_Y, gMapManager.GetMapViewRange(lpObj->Map)) != 0)
			{
				index = n;
			}

			if (index < 0)
			{
				continue;
			}

			if (MAP_ITEM_RANGE(index) == 0)
			{
				continue;
			}

			if (MAP_RANGE(lpObj->Map) == 0)
			{
				continue;
			}

			if (gMap[lpObj->Map].CheckItemGive(lpObj->Index, index) == 0)
			{
				continue;
			}

			CMapItem* lpItem = &gMap[lpObj->Map].m_Item[index];

			if (lpObj->PickupExc == 1 && lpItem->IsExcItem() != 0 || lpObj->PickupSocket == 1 && (FakeisJewels(lpItem->m_Index) == true) != 0)
			{
				PMSG_ITEM_GET_SEND pMsg;

				pMsg.header.set(0x22, sizeof(pMsg)); // Sửa setE thành set

				pMsg.result = 0xFF;

				memset(pMsg.ItemInfo, 0, sizeof(pMsg.ItemInfo));

				gObjFixInventoryPointer(lpObj->Index);

				if (lpItem->m_Index == GET_ITEM(14, 15)) // Money
				{

					gMap[lpObj->Map].ItemGive(lpObj->Index, index);

					if (gObjCheckMaxMoney(lpObj->Index, lpItem->m_BuyMoney) == 0)
					{
						lpObj->Money = MAX_MONEY;
					}
					else
					{
						lpObj->Money += lpItem->m_BuyMoney;
					}

					pMsg.result = 0xFE;

					pMsg.ItemInfo[0] = SET_NUMBERHB(SET_NUMBERHW(lpObj->Money));
					pMsg.ItemInfo[1] = SET_NUMBERLB(SET_NUMBERHW(lpObj->Money));
					pMsg.ItemInfo[2] = SET_NUMBERHB(SET_NUMBERLW(lpObj->Money));
					pMsg.ItemInfo[3] = SET_NUMBERLB(SET_NUMBERLW(lpObj->Money));

					DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
					continue;
				}

				CItem item = (*lpItem);

				if (gItemManager.InventoryInsertItemStack(lpObj, &item) != 0)
				{
					gMap[lpObj->Map].ItemGive(lpObj->Index, index);

					pMsg.result = 0xFD;

					gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

					DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
					continue;
				}

				pMsg.result = gItemManager.InventoryInsertItem(lpObj->Index, item);

				if (pMsg.result == 0xFF)
				{
					DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
					continue;
				}

				gMap[lpObj->Map].ItemGive(lpObj->Index, index);

				gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

				DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
			}

		}

		//Custom Pick
		for (int i = 0; i < MAX_CUSTOMPICK; i++)
		{
			if (lpObj->Pickup[i] == -1)
			{
				continue;
			}

			if (lpObj->Pickup[i] != gMap[lpObj->Map].m_Item[n].m_Index)
			{
				continue;
			}

			if (gViewport.CheckViewportObjectPosition(lpObj->Index, lpObj->Map, gMap[lpObj->Map].m_Item[n].m_X, gMap[lpObj->Map].m_Item[n].m_Y, gMapManager.GetMapViewRange(lpObj->Map)) != 0)
			{
				index = n;
			}

			if (index < 0)
			{
				continue;
			}

			if (MAP_ITEM_RANGE(index) == 0)
			{
				continue;
			}

			if (MAP_RANGE(lpObj->Map) == 0)
			{
				continue;
			}

			if (gMap[lpObj->Map].CheckItemGive(lpObj->Index, index) == 0)
			{
				continue;
			}

			CMapItem* lpItem = &gMap[lpObj->Map].m_Item[index];

			PMSG_ITEM_GET_SEND pMsg;

			pMsg.header.set(0x22, sizeof(pMsg)); // Sửa setE thành set

			pMsg.result = 0xFF;

			memset(pMsg.ItemInfo, 0, sizeof(pMsg.ItemInfo));

			gObjFixInventoryPointer(lpObj->Index);

			if (lpItem->m_Index == GET_ITEM(14, 15)) // Money
			{

				gMap[lpObj->Map].ItemGive(lpObj->Index, index);

				if (gObjCheckMaxMoney(lpObj->Index, lpItem->m_BuyMoney) == 0)
				{
					lpObj->Money = MAX_MONEY;
				}
				else
				{
					lpObj->Money += lpItem->m_BuyMoney;
				}

				pMsg.result = 0xFE;

				pMsg.ItemInfo[0] = SET_NUMBERHB(SET_NUMBERHW(lpObj->Money));
				pMsg.ItemInfo[1] = SET_NUMBERLB(SET_NUMBERHW(lpObj->Money));
				pMsg.ItemInfo[2] = SET_NUMBERHB(SET_NUMBERLW(lpObj->Money));
				pMsg.ItemInfo[3] = SET_NUMBERLB(SET_NUMBERLW(lpObj->Money));

				DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
				continue;
			}

			CItem item = (*lpItem);

			if (gItemManager.InventoryInsertItemStack(lpObj, &item) != 0)
			{
				gMap[lpObj->Map].ItemGive(lpObj->Index, index);

				pMsg.result = 0xFD;

				gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

				DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
				continue;
			}

			pMsg.result = gItemManager.InventoryInsertItem(lpObj->Index, item);

			if (pMsg.result == 0xFF)
			{
				DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
				continue;
			}

			gMap[lpObj->Map].ItemGive(lpObj->Index, index);

			gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

			DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
		}
	}
}


void CCustomPick::PickProc(LPOBJ lpObj) // OK
{

	if (lpObj->PickupEnable == 0)
	{
		return;
	}

	if (gMapManager.GetMapCustomPick(lpObj->Map) == 0)
	{
		this->OnPickClose(lpObj);
		gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(112, lpObj->Lang));
		gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(121, lpObj->Lang));
		return;
	}

	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0)
	{
		this->OnPickClose(lpObj);
		gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(112, lpObj->Lang));
		return;
	}

	if (lpObj->Interface.use != 0 || lpObj->Teleport != 0 || lpObj->DieRegen != 0)
	{
		this->OnPickClose(lpObj);
		gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(112, lpObj->Lang));
		return;
	}


	this->ItemGet(lpObj);

}

bool CCustomPick::CommandPick(LPOBJ lpObj, char* arg)
{
	if (lpObj->Interface.use != 0 || lpObj->Teleport != 0 || lpObj->DieRegen != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(122, lpObj->Lang));
		return 0;
	}

	if (gMapManager.GetMapCustomPick(lpObj->Map) == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(121, lpObj->Lang));
		return 0;
	}

	char mode[32] = { 0 };

	gCommandManager.GetString(arg, mode, sizeof(mode), 0);

	if (strcmp(mode, "exc") == 0)
	{
		if (lpObj->PickupExc == 1)
		{
			lpObj->PickupExc = 0;
			gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(111, lpObj->Lang), mode);

			gLog.Output(LOG_COMMAND, "[CustomPick][%s][%s] - (Disable: %s)", lpObj->Account, lpObj->Name, mode);
			return 0;
		}
		else
		{
			lpObj->PickupExc = 1;
			gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(110, lpObj->Lang), mode);

			lpObj->PickupEnable = 1;

			gLog.Output(LOG_COMMAND, "[CustomPick][%s][%s] - (Enable: %s)", lpObj->Account, lpObj->Name, mode);
			return 1;
		}
	}



	if (strcmp(mode, "ngoc") == 0)
	{
		if (lpObj->PickupSocket == 1)
		{
			lpObj->PickupSocket = 0;
			gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(111, lpObj->Lang), mode);

			gLog.Output(LOG_COMMAND, "[CustomPick][%s][%s] - (Disable: %s)", lpObj->Account, lpObj->Name, mode);
			return 0;
		}
		else
		{
			lpObj->PickupSocket = 1;
			gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(110, lpObj->Lang), mode);

			lpObj->PickupEnable = 1;

			gLog.Output(LOG_COMMAND, "[CustomPick][%s][%s] - (Enable: %s)", lpObj->Account, lpObj->Name, mode);
			return 1;
		}
	}

	if (strcmp(mode, "") != 0)
	{

		CUSTOMPICK_INFO CustomPickInfo;

		if (this->GetInfoByName(mode, &CustomPickInfo) == 0)
		{
			return 0;
		}

		if (lpObj->Pickup[CustomPickInfo.Index] == ((CustomPickInfo.Cat * 512) + CustomPickInfo.Item)) // Sửa 32 thành 512
		{
			lpObj->Pickup[CustomPickInfo.Index] = -1;
			gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(111, lpObj->Lang), mode);

			gLog.Output(LOG_COMMAND, "[CustomPick][%s][%s] - (Disable: %s)", lpObj->Account, lpObj->Name, mode);
			return 0;
		}
		else
		{
			lpObj->Pickup[CustomPickInfo.Index] = ((CustomPickInfo.Cat * 512) + CustomPickInfo.Item); // Sửa 32 thành 512
			gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(110, lpObj->Lang), mode);

			lpObj->PickupEnable = 1;

			gLog.Output(LOG_COMMAND, "[CustomPick][%s][%s] - (Enable: %s)", lpObj->Account, lpObj->Name, mode);
			return 1;
		}

	}
	return 1;

}

void CCustomPick::OnPickClose(LPOBJ lpObj) // OK
{
	lpObj->PickupEnable = 0;
	lpObj->PickupExc = 0;
	lpObj->PickupSocket = 0;
	lpObj->PickupSetItem = 0;

	for (int i = 0; i < MAX_CUSTOMPICK; i++)
	{
		lpObj->Pickup[i] = -1;
	}
}