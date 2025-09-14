#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"

class CustomStuff : public PlayerScript
{
public:
    CustomStuff() : PlayerScript("CustomStuff") {}

    void OnPlayerLeaveCombat(Player* player) override
    {
        if (sConfigMgr->GetOption<bool>("CustomStuff.Enable", false))
            RestorePlayerResources(player);
    }

    void OnPlayerFirstLogin(Player* player) override
    {
        if (sConfigMgr->GetOption<bool>("CustomStuff.Enable", false))
            ApplyStartingBonuses(player);
    }

    void OnPlayerLevelChanged(Player* player, uint8 oldLevel) override
    {
        if (sConfigMgr->GetOption<bool>("CustomStuff.Enable", false))
            SendCongratulationsMessage(player);
    }

private:
    const uint32 FROSTWEAVE_BAG = 41599;

    void RestorePlayerResources(Player* player)
    {
        player->SetHealth(player->GetMaxHealth());
        Powers p = player->getPowerType();
        player->SetPower(p, player->GetMaxPower(p));
    }

    void ApplyStartingBonuses(Player* player)
    {
        player->ModifyMoney(sConfigMgr->GetOption<uint32>("CustomStuff.StartingGold", 500) * GOLD);
        player->GiveLevel(sConfigMgr->GetOption<uint32>("CustomStuff.StartingLevel", 15));
        player->AddItem(FROSTWEAVE_BAG, 4);
    }

    void SendCongratulationsMessage(Player* player)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("GZ %s!", player->GetName().c_str());
    }
};

// Register your script(s) here
void AddCustomStuffScripts()
{
    new CustomStuff();
}
