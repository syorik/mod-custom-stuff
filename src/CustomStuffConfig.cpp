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

    startingGold = sConfigMgr->GetOption<uint32>("CustomStuff.StartingGold", 500u);
    startingLevel = sConfigMgr->GetOption<uint8>("CustomStuff.StartingLevel", static_cast<uint8>(20));
    levelUpGold = sConfigMgr->GetOption<uint32>("CustomStuff.LevelUpGold", 250u);

    startingProfessionCap = sConfigMgr->GetOption<uint16>("CustomStuff.StartingProffessionCap", static_cast<uint16>(150));
    startingProfessionValue = sConfigMgr->GetOption<uint16>("CustomStuff.StartingProffessionValue", static_cast<uint16>(75));
}


