#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"

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
        player->ModifyMoney(5000 * GOLD);
        player->GiveLevel(20);
        player->AddItem(FROSTWEAVE_BAG, 4);
    }
};

// Register your script(s) here
void AddCustomStuffScripts()
{
    new CustomStuff();
}
