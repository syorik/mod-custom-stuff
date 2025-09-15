#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "World.h"
#include "WorldSession.h"
#include "ObjectAccessor.h"

#include <sstream>

using namespace Acore::ChatCommands;

class CustomStuffCommand : public CommandScript
{
public:
    CustomStuffCommand() : CommandScript("CustomStuffCommand") {}

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable moduleCommandTable =
        {
            { "goto", HandleGotoCommand, SEC_PLAYER, Console::No },
            { "r", HandleRemoveResSicknessCommand, SEC_PLAYER, Console::No }
        };

        static ChatCommandTable commandTable =
        {
            { "custom", moduleCommandTable },
        };

        return commandTable;
    }

    static bool HandleRemoveResSicknessCommand(ChatHandler* handler, std::string /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (sConfigMgr->GetOption<bool>("CustomStuff.Enable", false) == false)
        {
            handler->SendSysMessage("CustomStuff module is disabled");
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Remove Resurrection Sickness (spell id 15007)
        player->RemoveAurasDueToSpell(15007);
        handler->SendSysMessage("Resurrection Sickness removed.");
        return true;
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

void AddCustomStuffCommandScripts()
{
    new CustomStuffCommand();
}


