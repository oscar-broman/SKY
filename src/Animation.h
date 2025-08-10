#pragma once

#include "CTypes.h"

namespace Animation
{
    namespace IDs
    {
        // PED_RUN animations
        constexpr WORD PED_RUN_START = 1222;
        constexpr WORD PED_RUN_END = 1236;
        constexpr WORD PED_SWAT_RUN = 1249;

        // PED_WOMAN animations
        constexpr WORD PED_WOMAN_START = 1275;
        constexpr WORD PED_WOMAN_END = 1287;

        // Other movement animations
        constexpr WORD FAT_FATRUN_ARMED = 459;
        constexpr WORD MUSCULAR_MUSCLERUN1 = 908;
        constexpr WORD MUSCULAR_MUSCLERUN2 = 909;
        constexpr WORD PED_WEAPON_CROUCH = 1274;
        constexpr WORD PED_WALK_PLAYER = 1266;
        constexpr WORD PED_SHOT_PARTIAL1 = 1241;
        constexpr WORD PED_SHOT_PARTIAL2 = 1242;

        // Baseball bat animations
        constexpr WORD BASEBALL_BAT_START = 17;
        constexpr WORD BASEBALL_BAT_END = 27;

        // Knife animations
        constexpr WORD KNIFE_START = 745;
        constexpr WORD KNIFE_END = 760;

        // Sword animations
        constexpr WORD SWORD_START = 1545;
        constexpr WORD SWORD_END = 1554;

        // Fight animations
        constexpr WORD FIGHT_START1 = 471;
        constexpr WORD FIGHT_END1 = 507;
        constexpr WORD FIGHT_START2 = 1135;
        constexpr WORD FIGHT_END2 = 1151;

        // Spray can animations
        constexpr WORD SPRAYCAN_START = 1160;
        constexpr WORD SPRAYCAN_END = 1167;

        // Grenade animations
        constexpr WORD GRENADE_START = 644;
        constexpr WORD GRENADE_END = 646;
    }

    bool IsProblematicAnimation(WORD animIndex);
}
