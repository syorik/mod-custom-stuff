#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "World.h"
#include "WorldSession.h"

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

class CustomStuffCommand : public CommandScript
{
public:
    CustomStuffCommand() : CommandScript("CustomStuffCommand") {}

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable moduleCommandTable =
        {
            { "goto", HandleGotoCommand, SEC_PLAYER, Console::No }
        };

        static ChatCommandTable commandTable =
        {
            { "custom", moduleCommandTable },
        };

        return commandTable;
    }

    static bool HandleGotoCommand(ChatHandler* handler, std::string args)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (sConfigMgr->GetOption<bool>("CustomStuff.Enable", false) == false)
        {
            handler->SendSysMessage("CustomStuff module is disabled");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (args.empty())
        {
            handler->SendSysMessage("Usage: .custom goto <player_name>");
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Check if player is in combat
        if (player->IsInCombat())
        {
            handler->SendSysMessage("You cannot teleport while in combat!");
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Check if player is in flight
        if (player->IsInFlight())
        {
            handler->SendSysMessage("You cannot teleport while flying!");
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Find target player
        Player* targetPlayer = ObjectAccessor::FindPlayerByName(args);
        if (!targetPlayer)
        {
            handler->SendSysMessage("Player not found or not online!");
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Don't allow teleporting to yourself
        if (targetPlayer == player)
        {
            handler->SendSysMessage("You cannot teleport to yourself!");
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Get target player's position
        float x = targetPlayer->GetPositionX();
        float y = targetPlayer->GetPositionY();
        float z = targetPlayer->GetPositionZ();
        float o = targetPlayer->GetOrientation();
        uint32 mapId = targetPlayer->GetMapId();

        // Teleport the player
        player->TeleportTo(mapId, x, y, z, o);

        // Send confirmation message
        std::ostringstream ss;
        ss << "Teleported to " << targetPlayer->GetName() << "!";
        handler->SendSysMessage(ss.str().c_str());

        return true;
    }
};

// Register your script(s) here
void AddCustomStuffScripts()
{
    new CustomStuff();
    new CustomStuffCommand();
}
