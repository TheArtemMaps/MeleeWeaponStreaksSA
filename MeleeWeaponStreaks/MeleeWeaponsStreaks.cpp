#include "plugin.h"
#include "CSpecialFX.h"
using namespace plugin;
class MeleeWeaponsStreaksSA {
public:
    MeleeWeaponsStreaksSA() {
        Events::gameProcessEvent += []() {
            CPlayerPed* playa = FindPlayerPed();
            auto weapType = playa->m_aWeapons[playa->m_nActiveWeaponSlot].m_eWeaponType;
            if (!playa)
                return;
            if (RpAnimBlendClumpGetAssociation(FindPlayerPed()->m_pRwClump, ANIM_BBBAT_1_BAT_1) ||
                RpAnimBlendClumpGetAssociation(FindPlayerPed()->m_pRwClump, ANIM_BBBAT_1_BAT_2) ||
                RpAnimBlendClumpGetAssociation(FindPlayerPed()->m_pRwClump, ANIM_BBBAT_1_BAT_3) ||
                RpAnimBlendClumpGetAssociation(FindPlayerPed()->m_pRwClump, ANIM_BBBAT_1_BAT_4) ||
                RpAnimBlendClumpGetAssociation(FindPlayerPed()->m_pRwClump, ANIM_BBBAT_1_BAT_PART) ||
                RpAnimBlendClumpGetAssociation(FindPlayerPed()->m_pRwClump, ANIM_BBBAT_1_BAT_BLOCK) ||
                RpAnimBlendClumpGetAssociation(FindPlayerPed()->m_pRwClump, ANIM_BBBAT_1_BAT_HIT_1) ||
                RpAnimBlendClumpGetAssociation(FindPlayerPed()->m_pRwClump, ANIM_BBBAT_1_BAT_HIT_2) ||
                RpAnimBlendClumpGetAssociation(FindPlayerPed()->m_pRwClump, ANIM_BBBAT_1_BAT_HIT_3))
            CSpecialFX::AddWeaponStreak(weapType);
        };
    }
} _MeleeWeaponsStreaksSA;
