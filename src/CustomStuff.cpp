#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"

// Add player scripts
class CustomStuff : public PlayerScript
{
public:
    CustomStuff() : PlayerScript("CustomStuff") { }

    void OnPlayerLeaveCombat(Player* player) override
    {
        if (sConfigMgr->GetOption<bool>("CustomStuff.Enable", false))
        {
            RestorePlayerResources(player);
        }
    }

private:
    void RestorePlayerResources(Player* player)
    {
        player->SetHealth(player->GetMaxHealth());
        
        Powers powerType = player->GetPowerType();
        player->SetPower(powerType, player->GetMaxPower(powerType));
    }
};

// Add all scripts in one
void AddCustomStuffScripts()
{
    new CustomStuff();
}
