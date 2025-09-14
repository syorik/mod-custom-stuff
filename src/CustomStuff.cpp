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
        if (sConfigMgr->GetOption<bool>("CustomStuff.Enable", false)) {
            SendCongratulationsMessage(player);
            ApplyAdditionalLevelUpBonuses(player);
        }
    }

private:
    const uint32 FROSTWEAVE_BAG = 41599;
    const uint32 DEFAULT_STARTING_GOLD = 500;
    const uint32 DEFAULT_STARTING_LEVEL = 15;
    const uint32 DEFAULT_LEVEL_UP_GOLD = 250;

    void RestorePlayerResources(Player* player)
    {
        player->SetHealth(player->GetMaxHealth());
        Powers p = player->getPowerType();
        player->SetPower(p, player->GetMaxPower(p));
    }

    void ApplyStartingBonuses(Player* player)
    {
        player->ModifyMoney(sConfigMgr->GetOption<uint32>("CustomStuff.StartingGold", DEFAULT_STARTING_GOLD) * GOLD);
        player->GiveLevel(sConfigMgr->GetOption<uint32>("CustomStuff.StartingLevel", DEFAULT_STARTING_LEVEL));
        player->AddItem(FROSTWEAVE_BAG, 4);
    }

    void SendCongratulationsMessage(Player* player)
    {
        std::ostringstream ss;
        ss << "Congrats on Level " << static_cast<int>(player->GetLevel()) << " " << player->GetName() << "! You've been awarded " << sConfigMgr->GetOption<uint32>("CustomStuff.LevelUpGold", DEFAULT_LEVEL_UP_GOLD) << " gold!";
        ChatHandler(player->GetSession()).SendNotification(SERVER_MSG_STRING, ss.str().c_str());
    }

    void ApplyAdditionalLevelUpBonuses(Player* player)
    {
        player->ModifyMoney(sConfigMgr->GetOption<uint32>("CustomStuff.LevelUpGold", DEFAULT_LEVEL_UP_GOLD) * GOLD);
    }
};

// Register your script(s) here
void AddCustomStuffScripts()
{
    new CustomStuff();
}
