#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "World.h"
#include "WorldSession.h"
#include "SpellMgr.h"

#include <sstream>
#include <vector>
#include <utility>

using namespace Acore::ChatCommands;

namespace {

enum SkillLine : uint32
{
    SKILL_ALCHEMY        = 171,
    SKILL_BLACKSMITHING  = 164,
    SKILL_ENCHANTING     = 333,
    SKILL_ENGINEERING    = 202,
    SKILL_HERBALISM      = 182,
    SKILL_INSCRIPTION    = 773,
    SKILL_JEWELCRAFTING  = 755,
    SKILL_LEATHERWORKING = 165,
    SKILL_MINING         = 186,
    SKILL_SKINNING       = 393,
    SKILL_TAILORING      = 197,

    // Secondary
    SKILL_COOKING        = 185,
    SKILL_FIRST_AID      = 129,
    SKILL_FISHING        = 356,
};

struct Prof
{
    uint32 skill;
    uint32 baseSpell; 
    std::vector<uint32> extraSpells; 
};

static const std::vector<Prof> kPrimaryProfs = {
    { SKILL_ALCHEMY,        2259,  {} },         
    { SKILL_BLACKSMITHING,  2018,  {} },         
    { SKILL_ENCHANTING,     7411,  {} },         
    { SKILL_ENGINEERING,    4036,  {} },         
    { SKILL_HERBALISM,      2366,  { 2383 } },    
    { SKILL_INSCRIPTION,    45357, {} },         
    { SKILL_JEWELCRAFTING,  25229, {} },         
    { SKILL_LEATHERWORKING, 2108,  {} },         
    { SKILL_MINING,         2575,  { 2580, 2656 } }, 
    { SKILL_SKINNING,       8613,  {} },         
    { SKILL_TAILORING,      3908,  {} },         
};

static const std::vector<Prof> kSecondaryProfs = {
    { SKILL_COOKING,   2550, {} }, 
    { SKILL_FIRST_AID, 3273, {} }, 
    { SKILL_FISHING,   7620, {} }, 
};

void LearnPlayerSpell(Player* player, uint32 spellId)
{
    if (!player->HasSpell(spellId))
        player->learnSpell(spellId, false);
}

void TeachAndSetSkill(Player* player, const Prof& p, uint16 value, uint16 cap)
{
    LearnPlayerSpell(player, p.baseSpell);
    for (uint32 extra : p.extraSpells)
        LearnPlayerSpell(player, extra);

    player->SetSkill(p.skill, /*step=*/0, /*value=*/value, /*max=*/cap);
}

}

class CustomStuff : public PlayerScript
{
public:
    CustomStuff() : PlayerScript("CustomStuff") {}

    void OnPlayerLeaveCombat(Player* player) override
    {
        Map *map = player->GetMap();
        if (IsModuleEnabled() && (map->IsDungeon() || map->IsRaid()))
            RestorePlayerResources(player);
    }

    void OnPlayerFirstLogin(Player* player) override
    {
        if (IsModuleEnabled())
        {
            ApplyStartingBonuses(player);
            TeachAllProfessions(player);
        }
    }

    void OnPlayerLevelChanged(Player* player, uint8 oldLevel) override
    {
        if (IsModuleEnabled()) {
            ApplyAdditionalLevelUpBonuses(player);
        }
    }

    bool OnPlayerBeforeTeleport(Player* player, uint32 mapId, float x, float y, float z, float o, uint32 options, Unit* target) override
    {
        if (IsModuleEnabled() && sConfigMgr->GetOption<bool>("CustomStuff.HearthstoneCooldownDisabled", false)) {
            ResetHearthstoneCooldown(player);
        }
        return true;
    }

    void OnPlayerLogin(Player* player) override
    {
        if (IsModuleEnabled() && sConfigMgr->GetOption<bool>("CustomStuff.HearthstoneCooldownDisabled", false)) {
            ResetHearthstoneCooldown(player);
        }
    }

private:
    const uint32 FROSTWEAVE_BAG = 41599;
    const uint32 DEFAULT_STARTING_GOLD = 500;
    const uint32 DEFAULT_STARTING_LEVEL = 15;
    const uint32 DEFAULT_LEVEL_UP_GOLD = 250;

    const uint32 HEARTHSTONER_SPELL = 8690;
    const uint32 NO_PLACE_LIKE_HOME_SPELL = 39937;

    bool IsModuleEnabled() {
        return sConfigMgr->GetOption<bool>("CustomStuff.Enable", false);
    }

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

    void ApplyAdditionalLevelUpBonuses(Player* player)
    {
        player->ModifyMoney(sConfigMgr->GetOption<uint32>("CustomStuff.LevelUpGold", DEFAULT_LEVEL_UP_GOLD) * GOLD);
    }

    void TeachAllProfessions(Player* player)
    {
        uint16 cap = sConfigMgr->GetOption<uint16>("CustomStuff.StartingProffessionCap", 75);
        uint16 value = sConfigMgr->GetOption<uint16>("CustomStuff.StartingProffessionValue", 75);

        for (auto const& p : kPrimaryProfs)
            TeachAndSetSkill(player, p, value, cap);

        for (auto const& p : kSecondaryProfs)
            TeachAndSetSkill(player, p, value, cap);

        ChatHandler(player->GetSession()).PSendSysMessage("All professions have been taught at Apprentice level.");
    }

    void ResetHearthstoneCooldown(Player* player)
    {
        removeSpellCooldown(player, HEARTHSTONER_SPELL);
        removeSpellCooldown(player, NO_PLACE_LIKE_HOME_SPELL);
    }

    void removeSpellCooldown(Player* player, uint32 spellId)
    {
        player->RemoveSpellCooldown(spellId, true);
        if (SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId))
        {
            if (info->GetCategory()) {
                player->RemoveCategoryCooldown(info->GetCategory(), true);
            }
        }
    }
};

void AddCustomStuffPlayerScripts()
{
    new CustomStuff();
}


