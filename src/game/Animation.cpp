#include "Animation.h"

namespace Animation
{
    bool IsProblematicAnimation(WORD animIndex)
    {
        using namespace IDs;

        return (animIndex >= PED_RUN_START && animIndex <= PED_RUN_END) ||
               (animIndex == PED_SWAT_RUN) ||
               (animIndex >= PED_WOMAN_START && animIndex <= PED_WOMAN_END) ||
               (animIndex == FAT_FATRUN_ARMED) ||
               (animIndex == MUSCULAR_MUSCLERUN1 || animIndex == MUSCULAR_MUSCLERUN2) ||
               (animIndex == PED_WEAPON_CROUCH) ||
               (animIndex == PED_WALK_PLAYER) ||
               (animIndex == PED_SHOT_PARTIAL1 || animIndex == PED_SHOT_PARTIAL2) ||
               (animIndex >= BASEBALL_BAT_START && animIndex <= BASEBALL_BAT_END) ||
               (animIndex >= KNIFE_START && animIndex <= KNIFE_END) ||
               (animIndex >= SWORD_START && animIndex <= SWORD_END) ||
               (animIndex >= FIGHT_START1 && animIndex <= FIGHT_END1) ||
               (animIndex >= FIGHT_START2 && animIndex <= FIGHT_END2);
    }
}