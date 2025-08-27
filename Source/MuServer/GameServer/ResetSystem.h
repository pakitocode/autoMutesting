#pragma once
#include "User.h"

struct RESET_INFO
{
    int Index;
    int MinReset;
    int MaxReset;
    int ReqLevel;
    int ReqMoney;
    int KeepStats;
    int AddPoints;
    int ReqChaos;
    int ReqBless;
    int ReqSoul;
    int ReqCreation;
    int ReqLoch;
};

class CResetSystem
{
public:
    CResetSystem();
    virtual ~CResetSystem();
    void Load(char* path);
    RESET_INFO* GetResetInfo(LPOBJ lpObj);
    bool CheckResetItem(LPOBJ lpObj, RESET_INFO* lpResetInfo);
    void TakeResetItem(LPOBJ lpObj, RESET_INFO* lpResetInfo);
    void RecalculateResetPoints(LPOBJ lpObj);
    int GetResetItemCount(LPOBJ lpObj, int ItemId); // Thêm hàm mới này
private:
    std::map<int, std::map<int, RESET_INFO>> m_ResetInfo;
};

extern CResetSystem gResetSystem;