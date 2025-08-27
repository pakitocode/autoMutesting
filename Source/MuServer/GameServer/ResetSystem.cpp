#include "stdafx.h"
#include "ResetSystem.h"
#include "ReadScript.h"
#include "Util.h"
#include "User.h"
#include "ItemManager.h"
#include "Path.h"
#include "Notice.h"
#include "Log.h"
#include "DefaultClassInfo.h"
#include "ObjectManager.h"

CResetSystem gResetSystem;

CResetSystem::CResetSystem()
{
	this->m_ResetInfo.clear();
}

CResetSystem::~CResetSystem()
{
}

// Dán code này vào file ResetSystem.cpp, thay thế hàm Load cũ
// Dán code này vào file ResetSystem.cpp, thay thế hàm Load cũ
void CResetSystem::Load(char* path)
{
    this->m_ResetInfo.clear();
    gLog.Output(LOG_GENERAL, "[ResetSystem] Bat dau nap file config reset...");

    for (int i = 0; i < 4; i++)
    {
        char filename[256];
        if (i == 0) { wsprintf(filename, "Reset.txt"); }
        else { wsprintf(filename, "Reset_AL%d.txt", i); }

        char fullpath[MAX_PATH];
        wsprintf(fullpath, gPath.GetFullPath(filename));

        CReadScript* lpReadScript = new CReadScript;

        if (lpReadScript->Load(fullpath))
        {
            this->m_ResetInfo[i].clear();
            try
            {
                eTokenResult token;
                while (true)
                {
                    token = lpReadScript->GetToken(); // Đọc token đầu tiên của mỗi dòng
                    if (token == TOKEN_END || token == TOKEN_END_SECTION)
                    {
                        break; // Nếu hết file/section thì dừng
                    }

                    RESET_INFO info = { 0 };
                    // Gán giá trị của token vừa đọc vào info.Index
                    info.Index = lpReadScript->GetNumber();
                    
                    // Đọc và gán các cột còn lại
                    info.MinReset = lpReadScript->GetAsNumber();
                    info.MaxReset = lpReadScript->GetAsNumber();
                    info.ReqLevel = lpReadScript->GetAsNumber();
                    info.ReqMoney = lpReadScript->GetAsNumber();
                    info.KeepStats = lpReadScript->GetAsNumber();
                    info.AddPoints = lpReadScript->GetAsNumber();
                    info.ReqChaos = lpReadScript->GetAsNumber();
                    info.ReqBless = lpReadScript->GetAsNumber();
                    info.ReqSoul = lpReadScript->GetAsNumber();
                    info.ReqCreation = lpReadScript->GetAsNumber();
                    info.ReqLoch = lpReadScript->GetAsNumber();
                    this->m_ResetInfo[i].insert(std::pair<int, RESET_INFO>(info.Index, info));
                }
                gLog.Output(LOG_GENERAL, "[ResetSystem] -> Doc du lieu cho AL%d thanh cong, tim thay %d dong.", i, this->m_ResetInfo[i].size());
            }
            catch (...)
            {
                gLog.Output(LOG_GENERAL, "[ResetSystem] -> !!! LOI !!! Gap su co khi doc noi dung file cho AL%d.", i);
            }
        }
        else
        {
            gLog.Output(LOG_GENERAL, "[ResetSystem] -> !!! THAT BAI !!! Khong tim thay hoac khong the mo file cho AL%d.", i);
        }
        delete lpReadScript;
    }
    gLog.Output(LOG_GENERAL, "[ResetSystem] Da nap xong toan bo file config reset.");
}

RESET_INFO* CResetSystem::GetResetInfo(LPOBJ lpObj)
{
	int AccountLevel = lpObj->AccountLevel;

	if (this->m_ResetInfo.find(AccountLevel) == this->m_ResetInfo.end() || this->m_ResetInfo[AccountLevel].empty())
	{
		AccountLevel = 0;
	}

	for (std::map<int, RESET_INFO>::iterator it = this->m_ResetInfo[AccountLevel].begin(); it != this->m_ResetInfo[AccountLevel].end(); it++)
	{
		if ((lpObj->Reset + 1) >= it->second.MinReset && (lpObj->Reset + 1) <= it->second.MaxReset)
		{
			return &it->second;
		}
	}
	return 0;
}

bool CResetSystem::CheckResetItem(LPOBJ lpObj, RESET_INFO* lpResetInfo)
{
    if (this->GetResetItemCount(lpObj, 399) < lpResetInfo->ReqChaos) return false;
    if (this->GetResetItemCount(lpObj, 461) < lpResetInfo->ReqBless) return false;
    if (this->GetResetItemCount(lpObj, 462) < lpResetInfo->ReqSoul) return false;
    if (this->GetResetItemCount(lpObj, 470) < lpResetInfo->ReqCreation) return false;
    if (this->GetResetItemCount(lpObj, 430) < lpResetInfo->ReqLoch) return false;
    return true;
}

void CResetSystem::TakeResetItem(LPOBJ lpObj, RESET_INFO* lpResetInfo)
{
	gItemManager.DeleteInventoryItemCount(lpObj, 399, -1, lpResetInfo->ReqChaos);
	gItemManager.DeleteInventoryItemCount(lpObj, 461, -1, lpResetInfo->ReqBless);
	gItemManager.DeleteInventoryItemCount(lpObj, 462, -1, lpResetInfo->ReqSoul);
	gItemManager.DeleteInventoryItemCount(lpObj, 470, -1, lpResetInfo->ReqCreation);
	gItemManager.DeleteInventoryItemCount(lpObj, 430, -1, lpResetInfo->ReqLoch);
}

// Dán vào file ResetSystem.cpp, thay thế hàm RecalculateResetPoints cũ
void CResetSystem::RecalculateResetPoints(LPOBJ lpObj)
{
	DWORD totalPoints = 0;
	int accountLevel = lpObj->AccountLevel;

	// --- Bước 1: Tính điểm thưởng từ các lần Reset ---
	if (this->m_ResetInfo.find(accountLevel) == this->m_ResetInfo.end() || this->m_ResetInfo[accountLevel].empty())
	{
		accountLevel = 0;
	}

	for (int currentReset = 1; currentReset <= lpObj->Reset; currentReset++)
	{
		for (std::map<int, RESET_INFO>::iterator it = this->m_ResetInfo[accountLevel].begin(); it != this->m_ResetInfo[accountLevel].end(); it++)
		{
			if (currentReset >= it->second.MinReset && currentReset <= it->second.MaxReset)
			{
				totalPoints += it->second.AddPoints;
				break;
			}
		}
	}

	// --- Bước 2: Cộng thêm điểm từ Trái Cây (Fruit) ---
	totalPoints += lpObj->FruitAddPoint;

	// Gán tổng số điểm cuối cùng cho nhân vật
	lpObj->LevelUpPoint = totalPoints;

}
int CResetSystem::GetResetItemCount(LPOBJ lpObj, int ItemId)
{
    int count = 0;
    for (int i = 0; i < INVENTORY_SIZE; ++i)
        {
        if (lpObj->Inventory[i].IsItem() && lpObj->Inventory[i].m_Index == ItemId)
            {
            count++;
            }
        }
    return count;
}