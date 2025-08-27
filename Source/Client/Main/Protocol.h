#pragma once

#include "ProtocolDefines.h"

//**********************************************//
//************ GameServer -> Client ************//
//**********************************************//

struct PMSG_NOTICE_RECV
{
	PBMSG_HEAD header; // C1:0D
	BYTE type;
	char message[256];
};

// --- Thêm vào file Protocol.h phía Client ---

struct PMSG_F8_AUTOPLAY_TOGGLE_SEND
{
    PBMSG_HEAD header; // Tên header có thể là C1_HEADER, PHEAD... tùy source của bạn
};

struct PMSG_VIEWPORT_DESTROY_RECV
{
	PBMSG_HEAD header; // C1:14
	BYTE count;
};

struct PMSG_VIEWPORT_DESTROY
{
	BYTE index[2];
};

struct PMSG_DAMAGE_RECV
{
	PBMSG_HEAD header; // C1:[PROTOCOL_CODE2] // 0x15
	BYTE index[2];
	BYTE damage[2];
	// Extra
	DWORD ViewCurHP;
	DWORD ViewDamageHP;
};

struct PMSG_USER_DIE_RECV
{
	PBMSG_HEAD header; // C1:17
	BYTE index[2];
	BYTE skill;
	BYTE killer[2];
};

struct PMSG_DURATION_SKILL_ATTACK_RECV
{
	PBMSG_HEAD header; // C3:1E
	BYTE skill;
	BYTE index[2];
	BYTE x;
	BYTE y;
	BYTE dir;
};

struct PMSG_ITEM_GET_RECV
{
	PBMSG_HEAD header; // C3:22
	BYTE result;
	BYTE ItemInfo[MAX_ITEM_INFO];
};

struct PMSG_LIFE_RECV
{
	PBMSG_HEAD header; // C1:26
	BYTE type;
	BYTE life[2];
	BYTE flag;
	// Extra
	DWORD ViewHP;
};

struct PMSG_MANA_RECV
{
	PBMSG_HEAD header; // C1:27
	BYTE type;
	BYTE mana[2];
	BYTE bp[2];
	// Extra
	DWORD ViewMP;
	DWORD ViewBP;
};

struct PMSG_FRUIT_RESULT_RECV
{
	PBMSG_HEAD header; // C1:2C
	BYTE result;
	// Extra
	DWORD ViewValue;
	DWORD ViewPoint;
	DWORD ViewStrength;
	DWORD ViewDexterity;
	DWORD ViewVitality;
	DWORD ViewEnergy;
};

struct PMSG_TRADE_OK_BUTTON_RECV
{
	PBMSG_HEAD header; // C1:3C
	BYTE flag;
};

struct PMSG_DEVIL_SQUARE_REQ_LEVELS_RECV
{
	PBMSG_HEAD header; // C1:8E
	int m_DevilSquareRequiredLevel[4][2];
};

struct PMSG_BLOOD_CASTLE_REQ_LEVELS_RECV
{
	PBMSG_HEAD header; // C1:8F
	int m_BloodCastleRequiredLevel[6][4];
};

struct PMSG_REWARD_EXPERIENCE_RECV
{
	PBMSG_HEAD header; // C1:9C
	BYTE index[2];
	WORD experience[2];
	BYTE damage[2];
	// Extra
	DWORD ViewDamageHP;
	DWORD ViewExperience;
	DWORD ViewNextExperience;
};

struct PMSG_QUEST_REWARD_RECV
{
	PBMSG_HEAD header; // C1:A3
	BYTE index[2];
	BYTE QuestReward;
	BYTE QuestAmount;
	// Extra
	DWORD ViewPoint;
};

struct PMSG_CHARACTER_DELETE_LEVEL_RECV
{
	PBMSG_HEAD header; // C1:DD
	WORD Level;
};

struct PMSG_CHARACTER_CREATION_ENABLE_RECV
{
	PBMSG_HEAD header; // C1:DE
	BYTE result;
};

struct PMSG_CHARACTER_MAX_LEVEL_RECV
{
	PBMSG_HEAD header; // C1:DF
	DWORD MaxCharacterLevel;
};

struct PMSG_CONNECT_CLIENT_RECV
{
	PSBMSG_HEAD header; // C1:F1:00
	BYTE result;
	BYTE index[2];
	BYTE ClientVersion[5];
};

struct PMSG_CONNECT_ACCOUNT_RECV
{
	PSBMSG_HEAD header; // C1:F1:01
	BYTE result;
};

struct PMSG_CLOSE_CLIENT_RECV
{
	PSBMSG_HEAD header; // C1:F1:02
	BYTE result;
};

struct PMSG_CHARACTER_LIST_RECV
{
	PSBMSG_HEAD header; // C1:F3:00
	BYTE count;
};

struct PMSG_CHARACTER_LIST
{
	BYTE slot;
	char Name[10];
	WORD Level;
	BYTE CtlCode;
	BYTE CharSet[11];
};

struct PMSG_CHARACTER_INFO_RECV
{
	PSBMSG_HEAD header; // C3:F3:03
	BYTE X;
	BYTE Y;
	BYTE Map;
	BYTE Dir;
	DWORD Experience;
	DWORD NextExperience;
	WORD LevelUpPoint;
	WORD Strength;
	WORD Dexterity;
	WORD Vitality;
	WORD Energy;
	WORD Life;
	WORD MaxLife;
	WORD Mana;
	WORD MaxMana;
	WORD BP;
	WORD MaxBP;
	DWORD Money;
	BYTE PKLevel;
	BYTE CtlCode;
	WORD FruitAddPoint;
	WORD MaxFruitAddPoint;
};

struct PMSG_CHARACTER_REGEN_RECV
{
	PSBMSG_HEAD header; // C3:F3:04
	BYTE X;
	BYTE Y;
	BYTE Map;
	BYTE Dir;
	WORD Life;
	WORD Mana;
	WORD BP;
	DWORD Experience;
	DWORD Money;
	// Extra
	DWORD ViewCurHP;
	DWORD ViewCurMP;
	DWORD ViewCurBP;
};

struct PMSG_LEVEL_UP_RECV
{
	PSBMSG_HEAD header; // C1:F3:05
	WORD Level;
	WORD LevelUpPoint;
	WORD MaxLife;
	WORD MaxMana;
	WORD MaxBP;
	WORD FruitAddPoint;
	WORD MaxFruitAddPoint;
	WORD FruitSubPoint;
	WORD MaxFruitSubPoint;
	// Extra
	DWORD ViewPoint;
	DWORD ViewMaxHP;
	DWORD ViewMaxMP;
	DWORD ViewMaxBP;
	DWORD ViewExperience;
	DWORD ViewNextExperience;
};

struct PMSG_LEVEL_UP_POINT_RECV
{
	PSBMSG_HEAD header; // C1:F3:06
	BYTE result;
	WORD MaxLifeAndMana;
	WORD MaxBP;
	// Extra
	DWORD ViewPoint;
	DWORD ViewMaxHP;
	DWORD ViewMaxMP;
	DWORD ViewMaxBP;
	DWORD ViewStrength;
	DWORD ViewDexterity;
	DWORD ViewVitality;
	DWORD ViewEnergy;
};

struct PMSG_MONSTER_DAMAGE_RECV
{
	PSBMSG_HEAD header; // C1:F3:07
	BYTE damage[2];
	// Extra
	DWORD ViewCurHP;
	DWORD ViewDamageHP;
};

struct PMSG_NEW_CHARACTER_INFO_RECV
{
	PSBMSG_HEAD header; // C1:F3:E0
	DWORD Level;
	DWORD LevelUpPoint;
	DWORD Experience;
	DWORD NextExperience;
	DWORD Strength;
	DWORD Dexterity;
	DWORD Vitality;
	DWORD Energy;
	DWORD Life;
	DWORD MaxLife;
	DWORD Mana;
	DWORD MaxMana;
	DWORD BP;
	DWORD MaxBP;
	DWORD FruitAddPoint;
	DWORD MaxFruitAddPoint;
	DWORD ViewReset;
	DWORD ViewGrandReset;
};

struct PMSG_NEW_CHARACTER_CALC_RECV
{
	PSBMSG_HEAD header; // C1:F3:E1
	DWORD ViewCurHP;
	DWORD ViewMaxHP;
	DWORD ViewCurMP;
	DWORD ViewMaxMP;
	DWORD ViewCurBP;
	DWORD ViewMaxBP;
	DWORD ViewPhysiSpeed;
	DWORD ViewMagicSpeed;
	DWORD ViewPhysiDamageMin;
	DWORD ViewPhysiDamageMax;
	DWORD ViewMagicDamageMin;
	DWORD ViewMagicDamageMax;
	DWORD ViewMagicDamageRate;
	DWORD ViewAttackSuccessRate;
	DWORD ViewDamageMultiplier;
	DWORD ViewDefense;
	DWORD ViewDefenseSuccessRate;
};

struct PMSG_HEALTH_BAR_RECV
{
	PSWMSG_HEAD header; // C2:F3:E2
	BYTE count;
};

//**********************************************//
//************ Client -> GameServer ************//
//**********************************************//

struct PMSG_LIVE_CLIENT_SEND
{
	PBMSG_HEAD header; // C3:0E
	DWORD TickCount;
	WORD PhysiSpeed;
	WORD MagicSpeed;
};

struct PMSG_SKILL_ATTACK_SEND
{
	PBMSG_HEAD header; // C3:19
	BYTE skill;
	BYTE index[2];
};

struct PMSG_DURATION_SKILL_ATTACK_SEND
{
	PBMSG_HEAD header; // C3:1E
	BYTE skill;
	BYTE x;
	BYTE y;
	BYTE dir;
	BYTE dis;
	BYTE angle;
	BYTE index[2];
};

struct PMSG_CONNECT_ACCOUNT_SEND
{
#pragma pack(1)
	PSBMSG_HEAD header; // C3:F1:01
	char account[10];
	char password[10];
	DWORD TickCount;
	BYTE ClientVersion[5];
	BYTE ClientSerial[16];
#pragma pack()
};

struct PMSG_SET_LANG_SEND
{
	PSBMSG_HEAD header; // C1:F1:04
	int lang;
};

struct PMSG_SET_HWID_SEND
{
	PSBMSG_HEAD header; // C1:F1:05
	char HardwareId[36];
};

struct PMSG_CHARACTER_LIST_SEND
{
	PSBMSG_HEAD header; // C1:F3:00
};

struct PMSG_CHARACTER_INFO_SEND
{
	PSBMSG_HEAD header; // C1:F3:03
	char name[10];
};

//**********************************************//
//**********************************************//
//**********************************************//

class CProtocol
{
public:

	CProtocol();

	virtual ~CProtocol();

	void Init();

private:

	static void HookProtocol();

	bool ProtocolCompiler(BYTE* lpMsg);

	bool TranslateProtocol(BYTE head, BYTE* lpMsg, int Size);

	void GCNoticeRecv(PMSG_NOTICE_RECV* lpMsg);

	void GCDamageRecv(PMSG_DAMAGE_RECV* lpMsg);

	void GCUserDieRecv(PMSG_USER_DIE_RECV* lpMsg);

	void GCDurationSkillAttackRecv(PMSG_DURATION_SKILL_ATTACK_RECV* lpMsg);

	void CGItemGetRecv(PMSG_ITEM_GET_RECV* lpMsg);

	void GCLifeRecv(PMSG_LIFE_RECV* lpMsg);

	void GCManaRecv(PMSG_MANA_RECV* lpMsg);

	void GCFruitResultRecv(PMSG_FRUIT_RESULT_RECV* lpMsg);

	void GCTradeOkButtonRecv(PMSG_TRADE_OK_BUTTON_RECV* lpMsg);

	void GCDevilSquareRequiredLevelsRecv(PMSG_DEVIL_SQUARE_REQ_LEVELS_RECV* lpMsg);

	void GCBloodCastleRequiredLevelsRecv(PMSG_BLOOD_CASTLE_REQ_LEVELS_RECV* lpMsg);

	void GCRewardExperienceRecv(PMSG_REWARD_EXPERIENCE_RECV* lpMsg);

	void GCQuestRewardRecv(PMSG_QUEST_REWARD_RECV* lpMsg);

	void GCCharacterDeleteMaxLevelRecv(PMSG_CHARACTER_DELETE_LEVEL_RECV* lpMsg);

	void GCCharacterCreationEnableRecv(PMSG_CHARACTER_CREATION_ENABLE_RECV* lpMsg);

	void GCCharacterMaxLevelRecv(PMSG_CHARACTER_MAX_LEVEL_RECV* lpMsg);

	void GCConnectClientRecv(PMSG_CONNECT_CLIENT_RECV* lpMsg);

	void GCConnectAccountRecv(PMSG_CONNECT_ACCOUNT_RECV* lpMsg);

	void GCCloseClientRecv(PMSG_CLOSE_CLIENT_RECV* lpMsg);

	void GCCharacterListRecv(PMSG_CHARACTER_LIST_RECV* lpMsg);

	void GCCharacterInfoRecv(PMSG_CHARACTER_INFO_RECV* lpMsg);

	void GCCharacterRegenRecv(PMSG_CHARACTER_REGEN_RECV* lpMsg);

	void GCLevelUpRecv(PMSG_LEVEL_UP_RECV* lpMsg);

	void GCLevelUpPointRecv(PMSG_LEVEL_UP_POINT_RECV* lpMsg);

	void GCMonsterDamageRecv(PMSG_MONSTER_DAMAGE_RECV* lpMsg);

	void GCNewCharacterInfoRecv(PMSG_NEW_CHARACTER_INFO_RECV* lpMsg);

	void GCNewCharacterCalcRecv(PMSG_NEW_CHARACTER_CALC_RECV* lpMsg);

	void GCHealthBarRecv(PMSG_HEALTH_BAR_RECV* lpMsg);

public:

	void DataSend(BYTE* lpMsg, DWORD size);

	void CGLiveClientSend();
};

extern CProtocol gProtocol;