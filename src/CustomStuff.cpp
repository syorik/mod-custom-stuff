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

private:
    void RestorePlayerResources(Player* player)
    {
        player->SetHealth(player->GetMaxHealth());
        Powers p = player->GetPowerType();
        player->SetPower(p, player->GetMaxPower(p));
    }
};

// Register your script(s) here
void AddCustomStuffScripts()
{
    new CustomStuff();
}
