#include "CustomStuffConfig.h"
#include "Config.h"

CustomStuffConfig::CustomStuffConfig()
{
    Reload();
}

void CustomStuffConfig::Reload()
{
    enable = sConfigMgr->GetOption<bool>("CustomStuff.Enable", false);
    hearthstoneCooldownDisabled = sConfigMgr->GetOption<bool>("CustomStuff.HearthstoneCooldownDisabled", false);

    startingGold = sConfigMgr->GetOption<uint32_t>("CustomStuff.StartingGold", 500u);
    startingLevel = sConfigMgr->GetOption<uint8_t>("CustomStuff.StartingLevel", static_cast<uint8_t>(20));
    levelUpGold = sConfigMgr->GetOption<uint32_t>("CustomStuff.LevelUpGold", 250u);

    startingProfessionCap = sConfigMgr->GetOption<uint16_t>("CustomStuff.StartingProffessionCap", static_cast<uint16_t>(150));
    startingProfessionValue = sConfigMgr->GetOption<uint16_t>("CustomStuff.StartingProffessionValue", static_cast<uint16_t>(75));
}


