// CustomAttack.cpp: implementation of the CCustomAttack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomAttack.h"
#include "CommandManager.h"
#include "EffectManager.h"
#include "GameMain.h"
#include "ItemManager.h"
#include "Util.h"
#include "Map.h"
#include "MapManager.h"
#include "Message.h"
#include "Notice.h"
#include "Party.h"
#include "ServerInfo.h"
#include "SkillManager.h"
#include "SocketManager.h"
#include "Viewport.h"
#include <sstream>
#include "ReadScript.h"
#include "Log.h"
#include "Attack.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomAttack::CCustomAttack() // OK
{

}

CCustomAttack::~CCustomAttack() // OK
{

}

void CCustomAttack::ReadCustomAttackInfo(char* section,char* path) // OK
{
	this->m_CustomAttackTime[0] = GetPrivateProfileInt(section,"CustomAttackTime_AL0",0,path);
	this->m_CustomAttackTime[1] = GetPrivateProfileInt(section,"CustomAttackTime_AL1",0,path);
	this->m_CustomAttackTime[2] = GetPrivateProfileInt(section,"CustomAttackTime_AL2",0,path);
	this->m_CustomAttackTime[3] = GetPrivateProfileInt(section,"CustomAttackTime_AL3",0,path);

	this->m_CustomAttackDelay = GetPrivateProfileInt(section,"CustomAttackDelay",0,path);

	this->m_CustomAttackPotionDelay = GetPrivateProfileInt(section,"CustomAttackAutoPotionDelay",0,path);

	this->m_CustomAttackAutoBuff[0] = GetPrivateProfileInt(section,"CustomAttackAutoBuff_AL0",0,path);
	this->m_CustomAttackAutoBuff[1] = GetPrivateProfileInt(section,"CustomAttackAutoBuff_AL1",0,path);
	this->m_CustomAttackAutoBuff[2] = GetPrivateProfileInt(section,"CustomAttackAutoBuff_AL2",0,path);
	this->m_CustomAttackAutoBuff[3] = GetPrivateProfileInt(section,"CustomAttackAutoBuff_AL3",0,path);

	this->m_CustomAttackAutoBuffDelay = GetPrivateProfileInt(section,"CustomAttackAutoBuffDelay",0,path);

	this->m_CustomAttackAutoResume[0] = GetPrivateProfileInt(section,"CustomAttackAutoResume_AL0",0,path);
	this->m_CustomAttackAutoResume[1] = GetPrivateProfileInt(section,"CustomAttackAutoResume_AL1",0,path);
	this->m_CustomAttackAutoResume[2] = GetPrivateProfileInt(section,"CustomAttackAutoResume_AL2",0,path);
	this->m_CustomAttackAutoResume[3] = GetPrivateProfileInt(section,"CustomAttackAutoResume_AL3",0,path);
}

bool CCustomAttack::CommandCustomAttack(LPOBJ lpObj, char* arg)
{	
	int SetSkillCommand = gCommandManager.GetNumber(arg, 0); // Pobranie ID umiej�tno�ci
	int SetBuffCommand = gCommandManager.GetNumber(arg, 1);   // Pobranie informacji o buffie

	SetSkillCommand = (SetSkillCommand > 0) ? SetSkillCommand : SKILL_NONE;
	int SkillNumberCommand = SKILL_NONE;

	if (lpObj->AttackCustom != 0) {
		this->OnAttackClose(lpObj);
		return 0;
	}

	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0) {
		gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(114, lpObj->Lang));
		return 0;
	}

	if (gMapManager.GetMapCustomAttack(lpObj->Map) == 0) {
		gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(121, lpObj->Lang));
		return 0;
	}

	std::string allowedSkillsForClass;

	if (lpObj->Class == CLASS_DW) {
		allowedSkillsForClass = "9,10,14"; // Tutaj wpisz ID dozwolonych umiej�tno�ci dla Dark Wizarda, oddzielone przecinkami.
	}
	else if (lpObj->Class == CLASS_DK) {
		allowedSkillsForClass = "41,42,43"; // Tutaj wpisz ID dozwolonych umiej�tno�ci dla Dark Knight, oddzielone przecinkami.
	}
	else if (lpObj->Class == CLASS_MG) {
		allowedSkillsForClass = "9,10,14,41,55,56"; // Tutaj wpisz ID dozwolonych umiej�tno�ci dla Magic Gladiator, oddzielone przecinkami.
	}
	else if (lpObj->Class == CLASS_FE) {
		allowedSkillsForClass = "24,52"; // Tutaj wpisz ID dozwolonych umiej�tno�ci dla Muse Elf, oddzielone przecinkami.
	}

	if (allowedSkillsForClass.empty()) {
		gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(123, lpObj->Lang));
		return 0;
	}

	std::vector<int> allowedSkillIDs;

	std::istringstream skillsStream(allowedSkillsForClass);
	std::string skillID;

	while (std::getline(skillsStream, skillID, ',')) {
		int allowedSkillID = std::stoi(skillID);
		allowedSkillIDs.push_back(allowedSkillID);
	}

	if (std::find(allowedSkillIDs.begin(), allowedSkillIDs.end(), SetSkillCommand) == allowedSkillIDs.end()) {
		gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(123, lpObj->Lang));
		return 0;
	}

	int SetSkill = gCommandManager.GetNumber(arg, 0);
	int SetBuff = gCommandManager.GetNumber(arg, 1);
	SetSkill = (SetSkill > 0) ? SetSkill : SKILL_NONE;
	int SkillNumber = SKILL_NONE;

	if (!lpObj->Inventory[0].IsItem())
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(115, lpObj->Lang));
		return 0; // Zablokuj akcj� customattack
	}


	/*if (lpObj->Class == CLASS_FE) // Je�li posta� to ELF
	{
		// Lista dozwolonych przedmiot�w w lewej r�ce dla ELF
		int allowedLeftHandItemIDs[] = { GET_ITEM(4, 0), GET_ITEM(4, 1), GET_ITEM(4, 2), GET_ITEM(4, 3), GET_ITEM(4, 4),
		GET_ITEM(4, 5), GET_ITEM(4, 6), GET_ITEM(4, 7), GET_ITEM(4, 8), GET_ITEM(4, 9), GET_ITEM(4, 10), GET_ITEM(4, 11),
		GET_ITEM(4, 12), GET_ITEM(4, 13), GET_ITEM(4, 14), GET_ITEM(4, 15), GET_ITEM(4, 16), GET_ITEM(4, 17), GET_ITEM(4, 18),
		GET_ITEM(4, 19), GET_ITEM(4, 20) }; // Dodaj ID przedmiot�w, kt�re s� dozwolone

		bool isAllowed = false;

		// Sprawd�, czy przedmiot w lewej r�ce znajduje si� na li�cie dozwolonych
		for (int i = 0; i < sizeof(allowedLeftHandItemIDs) / sizeof(allowedLeftHandItemIDs[0]); i++)
		{
			if (allowedLeftHandItemIDs[i] != -1 && lpObj->Inventory[0].m_Index == allowedLeftHandItemIDs[i])
			{
				isAllowed = true;
				break; // Je�li jest dozwolony, przerwij p�tl�
			}
		}

		// Je�li przedmiot nie jest na li�cie dozwolonych, zablokuj akcj� customattack
		if (!isAllowed)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(801, lpObj->Lang));
			return 0; // Zablokuj akcj� customattack
		}
	}*/

	if (this->GetAttackSkill(lpObj, &SkillNumber, SetSkill) == 0) {
		gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(115, lpObj->Lang));
		return 0;
	}

	if (SetBuff == 1 && this->m_CustomAttackAutoBuff[lpObj->AccountLevel] == 1) {
		lpObj->AttackCustomAutoBuff = 1;
		lpObj->AttackCustomAutoBuffDelay = GetTickCount() + (DWORD)this->m_CustomAttackAutoBuffDelay;
	}

	lpObj->AttackCustom = 1;
	lpObj->AttackCustomSkill = SkillNumber;
	lpObj->AttackCustomDelay = GetTickCount();
	lpObj->AttackCustomZoneX = lpObj->X;
	lpObj->AttackCustomZoneY = lpObj->Y;
	lpObj->AttackCustomZoneMap = lpObj->Map;
	this->DGCustomAttackResumeSaveSend(lpObj->Index);
	lpObj->AttackCustomOffline = 0;
	lpObj->AttackCustomOfflineTime = 0;
	lpObj->AttackCustomTime = this->m_CustomAttackTime[lpObj->AccountLevel] * 600;
	gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(118, lpObj->Lang));

	if (this->m_CustomAttackTime[lpObj->AccountLevel] > 0) {
		gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(116, lpObj->Lang), this->m_CustomAttackTime[lpObj->AccountLevel]);
	}


	return 1; // Zwr�cenie 1 oznacza pomy�lne wykonanie komendy

}


bool CCustomAttack::CommandCustomAttackOffline(LPOBJ lpObj,char* arg) // OK
{
	if(lpObj->AttackCustom == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,gMessage.GetTextMessage(119,lpObj->Lang));
		return 0;
	}

	if(gMap[lpObj->Map].CheckAttr(lpObj->X,lpObj->Y,1) != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,gMessage.GetTextMessage(120,lpObj->Lang));
		return 0;
	}

	if(DS_MAP_RANGE(lpObj->Map) != 0 || BC_MAP_RANGE(lpObj->Map) != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index,1,gMessage.GetTextMessage(120,lpObj->Lang));
		return 0;
	}

	lpObj->Socket = INVALID_SOCKET;

	lpObj->AttackCustomOffline = 1;

	lpObj->AttackCustomOfflineTime = 0;

	closesocket(lpObj->PerSocketContext->Socket);

	return 1;
}

// ========================================================================
// == HÀM GetAttackSkill (PHIÊN BẢN CUỐI CÙNG - CÓ LOG CHI TIẾT) ==
// ========================================================================
bool CCustomAttack::GetAttackSkill(LPOBJ lpObj, int* SkillNumber, int SetSkill)
{
    //gLog.Output(LOG_GENERAL, "[GetAttackSkill][%s] Bat dau tim skill. Skill dau vao (SetSkill): %d", lpObj->Name, SetSkill);

    // --- BƯỚC 1: Ưu tiên kiểm tra skill người chơi đang chọn/vừa dùng (SetSkill) ---
    if (SetSkill != -1 && SetSkill != 0)
    {
        CSkill* lpSkill = gSkillManager.GetSkill(lpObj, SetSkill);

        if (lpSkill != 0)
        {
            //gLog.Output(LOG_GENERAL, "[GetAttackSkill][%s] -> Da tim thay skill %d. Kiem tra tinh hop le...", lpObj->Name, SetSkill);
            switch(lpSkill->m_skill)
            {
                // Danh sách tất cả các skill tấn công có thể dùng để auto
                case SKILL_POISON:
                case SKILL_METEORITE:
                case SKILL_LIGHTNING:
                case SKILL_FIRE_BALL:
                case SKILL_FLAME:
                case SKILL_ICE:
                case SKILL_TWISTER:
                case SKILL_EVIL_SPIRIT:
                case SKILL_HELL_FIRE:
                case SKILL_POWER_WAVE:
                case SKILL_AQUA_BEAM:
                case SKILL_BLAST:
                case SKILL_INFERNO:
                case SKILL_FALLING_SLASH:
                case SKILL_LUNGE:
                case SKILL_UPPERCUT:
                case SKILL_CYCLONE:
                case SKILL_SLASH:
                case SKILL_TRIPLE_SHOT:
                case SKILL_TWISTING_SLASH:
                case SKILL_RAGEFUL_BLOW:
                case SKILL_DEATH_STAB:
                case SKILL_IMPALE:
                case SKILL_ICE_ARROW:
                case SKILL_PENETRATION:
                case SKILL_FIRE_SLASH:
                case SKILL_POWER_SLASH:
                    *SkillNumber = lpSkill->m_index;
                    //gLog.Output(LOG_GENERAL, "[GetAttackSkill][%s] -> -> OK! Skill %d hop le. Su dung skill nay.", lpObj->Name, *SkillNumber);
                    return true;
            }
             //gLog.Output(LOG_GENERAL, "[GetAttackSkill][%s] -> -> Loi: Skill %d khong co trong danh sach skill tan cong.", lpObj->Name, SetSkill);
        }
        else
        {
            //gLog.Output(LOG_GENERAL, "[GetAttackSkill][%s] -> -> Loi: Khong tim thay skill %d trong danh sach da hoc.", lpObj->Name, SetSkill);
        }
    }

    // --- BƯỚC 2: Nếu không có skill hợp lệ, tìm skill mặc định theo class ---
    //gLog.Output(LOG_GENERAL, "[GetAttackSkill][%s] -> Tim skill mac dinh cho class %d...", lpObj->Name, lpObj->Class);
    CSkill* lpDefaultSkill = 0;

    switch (lpObj->Class)
    {
        case CLASS_DW:
            lpDefaultSkill = gSkillManager.GetSkill(lpObj, SKILL_EVIL_SPIRIT);
            if (lpDefaultSkill == 0) lpDefaultSkill = gSkillManager.GetSkill(lpObj, SKILL_INFERNO);
            break;
        case CLASS_DK:
            lpDefaultSkill = gSkillManager.GetSkill(lpObj, SKILL_TWISTING_SLASH);
            if (lpDefaultSkill == 0) lpDefaultSkill = gSkillManager.GetSkill(lpObj, SKILL_DEATH_STAB);
            break;
        case CLASS_FE:
            lpDefaultSkill = gSkillManager.GetSkill(lpObj, SKILL_TRIPLE_SHOT);
            if (lpDefaultSkill == 0) lpDefaultSkill = gSkillManager.GetSkill(lpObj, SKILL_PENETRATION);
            break;
        case CLASS_MG:
            if (lpObj->Inventory[0].IsItem() && lpObj->Inventory[0].m_Index >= GET_ITEM(5, 0) && lpObj->Inventory[0].m_Index < GET_ITEM(6, 0)) { // Nếu cầm gậy
                lpDefaultSkill = gSkillManager.GetSkill(lpObj, SKILL_EVIL_SPIRIT);
            } else { // Nếu cầm kiếm
                lpDefaultSkill = gSkillManager.GetSkill(lpObj, SKILL_POWER_SLASH);
                 if (lpDefaultSkill == 0) lpDefaultSkill = gSkillManager.GetSkill(lpObj, SKILL_TWISTING_SLASH);
            }
            break;
    }

    if (lpDefaultSkill != 0)
    {
        *SkillNumber = lpDefaultSkill->m_index;
        //gLog.Output(LOG_GENERAL, "[GetAttackSkill][%s] -> -> Tim thay skill mac dinh: %d", lpObj->Name, *SkillNumber);
        return true;
    }

    //gLog.Output(LOG_GENERAL, "[GetAttackSkill][%s] -> That bai, khong tim thay bat ky skill nao phu hop.", lpObj->Name);
    return false;
}

bool CCustomAttack::GetTargetMonster(LPOBJ lpObj,int SkillNumber,int* MonsterIndex) // OK
{
	int NearestDistance = 100;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE || OBJECT_RANGE(lpObj->VpPlayer2[n].index) == 0 || lpObj->VpPlayer2[n].type != OBJECT_MONSTER)
		{
			continue;
		}

		if(gSkillManager.CheckSkillTarget(lpObj,lpObj->VpPlayer2[n].index,-1,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(gObjCalcDistance(lpObj,&gObj[lpObj->VpPlayer2[n].index]) >= NearestDistance)
		{
			continue;
		}

		if(gSkillManager.CheckSkillRange(SkillNumber,lpObj->X,lpObj->Y,gObj[lpObj->VpPlayer2[n].index].X,gObj[lpObj->VpPlayer2[n].index].Y) != 0)
		{
			(*MonsterIndex) = lpObj->VpPlayer2[n].index;
			NearestDistance = gObjCalcDistance(lpObj,&gObj[lpObj->VpPlayer2[n].index]);
			continue;
		}

		if(gSkillManager.CheckSkillRadio(SkillNumber,lpObj->X,lpObj->Y,gObj[lpObj->VpPlayer2[n].index].X,gObj[lpObj->VpPlayer2[n].index].Y) != 0)
		{
			(*MonsterIndex) = lpObj->VpPlayer2[n].index;
			NearestDistance = gObjCalcDistance(lpObj,&gObj[lpObj->VpPlayer2[n].index]);
			continue;
		}
	}



	return ((NearestDistance==100)?0:1);
}

void CCustomAttack::OnAttackClose(LPOBJ lpObj)
{
	if (lpObj->AttackCustom != 0)
	{

		lpObj->AttackCustom = 0;
		lpObj->AttackCustomSkill = 0;
		lpObj->AttackCustomDelay = 0;
		lpObj->AttackCustomZoneX = 0;
		lpObj->AttackCustomZoneY = 0;
		lpObj->AttackCustomZoneMap = 0;
		lpObj->AttackCustomAutoBuff = 0;
		gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(119, lpObj->Lang));

		this->DGCustomAttackResumeSaveSend(lpObj->Index);
	}

	if (lpObj->AttackCustomOffline == 1)
	{
		lpObj->AttackCustomOffline = 2;
		lpObj->AttackCustomOfflineTime = 5;
	}
}


void CCustomAttack::OnAttackSecondProc(LPOBJ lpObj)
{
	if (lpObj->AttackCustomOffline != 0)
	{
		if (lpObj->AttackCustomOffline == 2)
		{
			if ((--lpObj->AttackCustomOfflineTime) == 0)
			{
				gObjDel(lpObj->Index);
				lpObj->AttackCustomOffline = 0;
				lpObj->AttackCustomOfflineTime = 0;
				//lpObj->AttackCustomOfflineMoneyDelay = 0;
			}
		}

		lpObj->CheckSumTime = GetTickCount();
		lpObj->ConnectTickCount = GetTickCount();
	}

	if (lpObj->AttackCustom == 1 && gMapManager.GetMapCustomAttack(lpObj->Map) == 0)
	{
		this->OnAttackClose(lpObj);
		gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(121, lpObj->Lang));
	}

	if (lpObj->Class == CLASS_FE) {
		if (lpObj->AttackCustom == 1)
		{
			if (!lpObj->Inventory[0].IsItem() || !lpObj->Inventory[1].IsItem())
			{
				this->OnAttackClose(lpObj);
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(122, lpObj->Lang));
				lpObj->AttackCustom = 0; // Wy��cz komend�, je�li brak broni w lewej r�ce
			}
			else if (lpObj->Inventory[1].m_Index != GET_ITEM(4, 0) &&
				lpObj->Inventory[1].m_Index != GET_ITEM(4, 1) &&
				lpObj->Inventory[1].m_Index != GET_ITEM(4, 2) &&
				lpObj->Inventory[1].m_Index != GET_ITEM(4, 3) &&
				lpObj->Inventory[1].m_Index != GET_ITEM(4, 4) &&
				lpObj->Inventory[1].m_Index != GET_ITEM(4, 5) &&
				lpObj->Inventory[1].m_Index != GET_ITEM(4, 6) &&
				lpObj->Inventory[1].m_Index != GET_ITEM(4, 7) &&
				lpObj->Inventory[1].m_Index != GET_ITEM(4, 17))
			{
				this->OnAttackClose(lpObj);
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(122, lpObj->Lang)); // Odpowiedni notice, gdy w prawej r�ce nie ma odpowiedniej broni
				lpObj->AttackCustom = 0; // Wy��cz komend�, je�li w prawej r�ce nie ma odpowiedniej broni
			}
		}
	}

	if (lpObj->Class == CLASS_FE) {
		if (lpObj->AttackCustom == 1) {
			bool hasItem4_7 = false;
			bool hasItem4_15 = false;

			if (lpObj->Inventory[0].IsItem()) {
				if (lpObj->Inventory[0].m_Index == GET_ITEM(4, 7) || lpObj->Inventory[0].m_Index == GET_ITEM(4, 15)) {
					hasItem4_7 = true;
				}
			}

			if (lpObj->Inventory[1].IsItem()) {
				if (lpObj->Inventory[1].m_Index == GET_ITEM(4, 7) || lpObj->Inventory[1].m_Index == GET_ITEM(4, 15)) {
					hasItem4_15 = true;
				}
			}

			if (hasItem4_7 && hasItem4_15) {
				// Je�li posta� ma naraz oba przedmioty 4,7 i 4,15 w obu r�kach lub w jednej r�ce
				this->OnAttackClose(lpObj);
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(122, lpObj->Lang));
				lpObj->AttackCustom = 0; // Wy��cz komend� customattack
			}
		}
	}


	if (lpObj->AttackCustom == 1)
	{
		if (!lpObj->Inventory[0].IsItem())
		{
			this->OnAttackClose(lpObj);
			gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(122, lpObj->Lang));
			lpObj->AttackCustom = 0; // Wy��cz komend�, je�li brak broni w lewej r�ce
		}

	}

}

void CCustomAttack::OnAttackAlreadyConnected(LPOBJ lpObj) // OK
{
	if(lpObj->AttackCustomOffline != 0)
	{
		gObjDel(lpObj->Index);
		lpObj->AttackCustomOffline = 0;
		lpObj->AttackCustomOfflineTime = 0;
		//lpObj->AttackCustomOfflineMoneyDelay = 0;
	}
}

void CCustomAttack::OnAttackMonsterAndMsgProc(LPOBJ lpObj) // OK
{
	CSkill* lpSkill;

	if(lpObj->AttackCustom == 0)
	{
		return;
	}
	//gLog.Output(LOG_GENERAL, "[DEBUG_AUTOPLAY][%s] Bat dau xu ly tan cong.", lpObj->Name);


	//if(this->CheckRequireMoney(lpObj) == 0)
	//{
	//	this->OnAttackClose(lpObj);
	//	return;
	//}

	if(gMap[lpObj->Map].CheckAttr(lpObj->X,lpObj->Y,1) != 0)
	{
		//gLog.Output(LOG_GENERAL, "[DEBUG_AUTOPLAY][%s] Dung lai vi dang o trong thanh.", lpObj->Name);
		this->OnAttackClose(lpObj);
		return;
	}

	if((lpSkill=gSkillManager.GetSkill(lpObj,lpObj->AttackCustomSkill)) == 0)
	{
		//gLog.Output(LOG_GENERAL, "[DEBUG_AUTOPLAY][%s] Loi: Khong tim thay skill ID %d.", lpObj->Name, lpObj->AttackCustomSkill);
		this->OnAttackClose(lpObj);
		return;
	}

	if (lpObj->AttackCustomTime > 0) 
	{
		if((--lpObj->AttackCustomTime) == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index,1,gMessage.GetTextMessage(117,lpObj->Lang));
			this->OnAttackClose(lpObj);
			return;
		}
	}

	if(gViewport.CheckViewportObjectPosition(lpObj->Index,lpObj->AttackCustomZoneMap,lpObj->AttackCustomZoneX,lpObj->AttackCustomZoneY,5) == 0)
	{
		//gLog.Output(LOG_GENERAL, "[DEBUG_AUTOPLAY][%s] Dung lai vi di qua xa vi tri ban dau.", lpObj->Name);
		this->OnAttackClose(lpObj);
		return;
	}

	if((GetTickCount()-((DWORD)lpObj->PotionTime)) >= (DWORD)this->m_CustomAttackPotionDelay && ((lpObj->Life*100)/(lpObj->MaxLife+lpObj->AddLife)) < 50)
	{
		PMSG_ITEM_USE_RECV pMsg;

		pMsg.header.set(0x26,sizeof(pMsg));

		pMsg.SourceSlot = 0xFF;

		pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager.GetInventoryItemSlot(lpObj,GET_ITEM(14,3),-1):pMsg.SourceSlot);

		pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager.GetInventoryItemSlot(lpObj,GET_ITEM(14,2),-1):pMsg.SourceSlot);

		pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager.GetInventoryItemSlot(lpObj,GET_ITEM(14,1),-1):pMsg.SourceSlot);

		pMsg.TargetSlot = 0xFF;

		//pMsg.type = 0;

		if(INVENTORY_RANGE(pMsg.SourceSlot) != 0){gItemManager.CGItemUseRecv(&pMsg,lpObj->Index);}
	}

	if((GetTickCount()-((DWORD)lpObj->PotionTime)) >= (DWORD)this->m_CustomAttackPotionDelay && ((lpObj->Mana*100)/(lpObj->MaxMana+lpObj->AddMana)) < 50)
	{
		PMSG_ITEM_USE_RECV pMsg;

		pMsg.header.set(0x26,sizeof(pMsg));

		pMsg.SourceSlot = 0xFF;

		pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager.GetInventoryItemSlot(lpObj,GET_ITEM(14,6),-1):pMsg.SourceSlot);

		pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager.GetInventoryItemSlot(lpObj,GET_ITEM(14,5),-1):pMsg.SourceSlot);

		pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager.GetInventoryItemSlot(lpObj,GET_ITEM(14,4),-1):pMsg.SourceSlot);

		pMsg.TargetSlot = 0xFF;

		//pMsg.type = 0;

		if(INVENTORY_RANGE(pMsg.SourceSlot) != 0){gItemManager.CGItemUseRecv(&pMsg,lpObj->Index);}
	}

	

	// --- THAY THẾ KHỐI IF KIỂM TRA MANA BẰNG KHỐI NÀY ---

    // Kiểm tra xem có đủ mana để dùng skill đã chọn không
    if(lpObj->Mana < gSkillManager.GetSkillMana(lpSkill->m_index))
    {
        // Nếu KHÔNG đủ mana, thử lấy skill "đánh thường" (ID = 0)
        lpSkill = gSkillManager.GetSkill(lpObj, 0); 
        
        // Nếu vì lý do nào đó không lấy được cả skill đánh thường, thì mới thoát
        if (lpSkill == 0)
        {
            return;
        }
    }

	if((GetTickCount()-((DWORD)lpObj->AttackCustomDelay)) >= (((((DWORD)lpObj->PhysiSpeed)*5)>1500)?0:(1500-(((DWORD)lpObj->PhysiSpeed)*5))))
	{
		if ((GetTickCount()-((DWORD)lpObj->AttackCustomDelay)) < (DWORD)this->m_CustomAttackDelay)
		{
			return;
		}
		//gLog.Output(LOG_GENERAL, "[DEBUG_AUTOPLAY][%s] Du dieu kien tan cong, bat dau tim quai.", lpObj->Name);

		//this->ItemGet(lpObj);

		int MonsterIndex = -1;

		lpObj->AttackCustomDelay = GetTickCount();

		// === THAY THẾ TOÀN BỘ KHỐI IF/ELSE CŨ BẰNG KHỐI NÀY ===

		if(this->GetTargetMonster(lpObj,lpSkill->m_index,&MonsterIndex) != 0)
		{
			// BƯỚC 1: Gửi tín hiệu về Client để "vẽ" hiệu ứng skill
			// Hàm này sẽ thông báo cho tất cả người chơi xung quanh thấy bạn đang ra skill
			gSkillManager.GCSkillAttackSend(lpObj, lpSkill->m_index, MonsterIndex, 1);
		
			// BƯỚC 2: Ra lệnh cho Server thực sự tấn công và tính sát thương
			// Hàm này đảm bảo quái vật sẽ mất máu và bạn nhận được kinh nghiệm
			gAttack.Attack(lpObj, &gObj[MonsterIndex], lpSkill, true, 0, 0);
		}
		else
		{
		// Giữ lại log để debug nếu cần
		// gLog.Output(LOG_GENERAL, "[DEBUG_AUTOPLAY][%s] Khong tim thay quai nao trong tam.", lpObj->Name);
		}
	}
}

void CCustomAttack::SendSkillAttack(LPOBJ lpObj,int aIndex,int SkillNumber) // OK
{
	PMSG_SKILL_ATTACK_RECV pMsg;

	pMsg.header.set(0x19,sizeof(pMsg));

	#if(GAMESERVER_UPDATE>=701)

	pMsg.skillH = SET_NUMBERHB(SkillNumber);

	pMsg.skillL = SET_NUMBERLB(SkillNumber);

	pMsg.indexH = SET_NUMBERHB(aIndex);

	pMsg.indexL = SET_NUMBERLB(aIndex);

	#else

	pMsg.skill = SkillNumber;

	pMsg.index[0] = SET_NUMBERHB(aIndex);

	pMsg.index[1] = SET_NUMBERLB(aIndex);

	#endif

	//pMsg.dis = 0;

	gSkillManager.CGSkillAttackRecv(&pMsg,lpObj->Index);
}

void CCustomAttack::SendMultiSkillAttack(LPOBJ lpObj,int aIndex,int SkillNumber) // OK
{
	//gLog.Output(LOG_GENERAL, "[DEBUG_AUTOPLAY][%s] Da vao ben trong ham SendMultiSkillAttack.", lpObj->Name); // << THÊM DÒNG NÀY
	this->SendDurationSkillAttack(lpObj,aIndex,SkillNumber);

	BYTE send[256];

	PMSG_MULTI_SKILL_ATTACK_RECV pMsg;

	pMsg.header.set(PROTOCOL_CODE4,sizeof(pMsg));

	int size = sizeof(pMsg);

	#if(GAMESERVER_UPDATE>=701)

	pMsg.skillH = SET_NUMBERHB(SkillNumber);

	pMsg.skillL = SET_NUMBERLB(SkillNumber);

	#else

	pMsg.skill = SkillNumber;

	#endif

	pMsg.x = (BYTE)lpObj->X;

	pMsg.y = (BYTE)lpObj->Y;

	pMsg.serial = 0;

	pMsg.count = 0;

	PMSG_MULTI_SKILL_ATTACK info;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE || OBJECT_RANGE(lpObj->VpPlayer2[n].index) == 0 || lpObj->VpPlayer2[n].type != OBJECT_MONSTER)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(gSkillManager.CheckSkillTarget(lpObj,index,aIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(gSkillManager.CheckSkillRadio(SkillNumber,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		#if(GAMESERVER_UPDATE>=701)

		info.indexH = SET_NUMBERHB(index);

		info.indexL = SET_NUMBERLB(index);

		#else

		info.index[0] = SET_NUMBERHB(index);

		info.index[1] = SET_NUMBERLB(index);

		#endif

		//info.MagicKey = 0;

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		if(CHECK_SKILL_ATTACK_COUNT(pMsg.count) == 0)
		{
			break;
		}
	}

	pMsg.header.size = size;

	memcpy(send,&pMsg,sizeof(pMsg));

	gSkillManager.CGMultiSkillAttackRecv((PMSG_MULTI_SKILL_ATTACK_RECV*)send,lpObj->Index,0);
}

void CCustomAttack::SendDurationSkillAttack(LPOBJ lpObj,int aIndex,int SkillNumber) // OK
{
	PMSG_DURATION_SKILL_ATTACK_RECV pMsg;

	pMsg.header.set(0x1E,sizeof(pMsg));

	#if(GAMESERVER_UPDATE>=701)

	pMsg.skillH = SET_NUMBERHB(SkillNumber);

	pMsg.skillL = SET_NUMBERLB(SkillNumber);

	#else

	pMsg.skill = SkillNumber;

	#endif

	pMsg.x = (BYTE)gObj[aIndex].X;

	pMsg.y = (BYTE)gObj[aIndex].Y;

	pMsg.dir = (gSkillManager.GetSkillAngle(gObj[aIndex].X,gObj[aIndex].Y,lpObj->X,lpObj->Y)*255)/360;

	pMsg.dis = 0;

	pMsg.angle = (gSkillManager.GetSkillAngle(lpObj->X,lpObj->Y,gObj[aIndex].X,gObj[aIndex].Y)*255)/360;

	#if(GAMESERVER_UPDATE>=803)

	pMsg.indexH = SET_NUMBERHB(aIndex);

	pMsg.indexL = SET_NUMBERLB(aIndex);

	#else

	pMsg.index[0] = SET_NUMBERHB(aIndex);
	
	pMsg.index[1] = SET_NUMBERLB(aIndex);

	#endif

	//pMsg.MagicKey = 0;

	gSkillManager.CGDurationSkillAttackRecv(&pMsg,lpObj->Index);
}



void CCustomAttack::DGCustomAttackResumeSend(int aIndex) // OK
{
	
	LPOBJ lpObj = &gObj[aIndex];

	if (this->m_CustomAttackAutoResume[lpObj->AccountLevel] == 0)
	{
		return;
	}

	SDHP_CARESUME_SEND pMsg;

	pMsg.header.set(0x08, 0x00,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.name,lpObj->Name,sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg)); 

}

void CCustomAttack::DGCustomAttackResumeRecv(SDHP_CARESUME_RECV* lpMsg) // OK
{

	LPOBJ lpObj = &gObj[lpMsg->index];

	if(gObjIsConnectedGP(lpMsg->index) == 0)
	{
		return;
	}

	if(strcmp(lpMsg->name,lpObj->Name) != 0)
	{
		return;
	}

	lpObj->AttackCustom = lpMsg->active;

	lpObj->AttackCustomSkill = lpMsg->skill;

	lpObj->AttackCustomDelay = GetTickCount();

	lpObj->AttackCustomZoneX = lpMsg->posx;

	lpObj->AttackCustomZoneY = lpMsg->posy;

	lpObj->AttackCustomZoneMap = lpMsg->map;

	lpObj->AttackCustomAutoBuff = lpMsg->autobuff;

}

void CCustomAttack::DGCustomAttackResumeSaveSend(int Index)
{
	
	LPOBJ lpObj = &gObj[Index];

	if (this->m_CustomAttackAutoResume[lpObj->AccountLevel] == 0)
	{
		return;
	}

	SDHP_CARESUME_SAVE_SEND pMsg;

	pMsg.header.set(0x08, 0x01,sizeof(pMsg));

	pMsg.index = lpObj->Index;

	memcpy(pMsg.name,lpObj->Name,sizeof(pMsg.name));

	pMsg.active = lpObj->AttackCustom;

	pMsg.skill = lpObj->AttackCustomSkill;

	pMsg.posx = lpObj->AttackCustomZoneX;

	pMsg.posy = lpObj->AttackCustomZoneY;

	pMsg.map = lpObj->AttackCustomZoneMap;

	pMsg.autobuff = lpObj->AttackCustomAutoBuff;

	pMsg.offpvp = lpObj->DisablePvp;

	pMsg.autoreset = lpObj->AutoResetEnable;

	pMsg.autoaddstr = lpObj->AutoResetStats[0];

	pMsg.autoaddagi = lpObj->AutoResetStats[1];

	pMsg.autoaddvit = lpObj->AutoResetStats[2];

	pMsg.autoaddene = lpObj->AutoResetStats[3];

	pMsg.autoaddcmd = lpObj->AutoResetStats[4];

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg)); 

}
