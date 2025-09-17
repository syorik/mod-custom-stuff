#pragma once

#include <cstdint>

class CustomStuffConfig
{
public:
    static CustomStuffConfig& Get()
    {
        static CustomStuffConfig instance;
        return instance;
    }

    // Call to refresh values from sConfigMgr at runtime if needed
    void Reload();

    // Cached options
    bool enable = false;
    bool hearthstoneCooldownDisabled = false;

    uint32_t startingGold = 500;
    uint8_t startingLevel = 20;
    uint32_t levelUpGold = 250;

    uint16_t startingProfessionCap = 150;
    uint16_t startingProfessionValue = 75;

private:
    CustomStuffConfig();
};


