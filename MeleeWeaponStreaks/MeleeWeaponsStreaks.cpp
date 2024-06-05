#include "plugin.h"
#include "CSpecialFX.h"
#include "CMotionBlurStreaks.h"
#include "ini.h"
#include <format>
#include <csignal>
#include "IniReader.h"
#include <filesystem>
#include "CCheat.h"
#include "CHud.h"
using namespace plugin;

#define RpAtomicGetFrameMacro(_atomic) ((RwFrame *) rwObjectGetParent(_atomic))
#define RpAtomicGetFrame(_atomic) RpAtomicGetFrameMacro(_atomic)

void RegStreak(uint32_t id, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, CVector leftPoint, CVector rightPoint) {
    plugin::Call<0x721DC0, uint32_t, uint8_t, uint8_t, uint8_t, uint8_t, CVector, CVector>(id, red, green, blue, alpha, leftPoint, rightPoint);
}
static bool TestCheat(std::string const& cheat) {
    std::string str = cheat;
    std::reverse(str.begin(), str.end());
    if (!str.compare(0, str.size(), CCheat::m_CheatString, str.size())) {
        CCheat::m_CheatString[0] = '\0';
        return true;
    }
    return false;
}
uint8_t r[512], g[512], b[512], a[512];
float xstart[512], ystart[512], zstart[512], xend[512], yend[512], zend[512];
bool weaponInitialized[512] = { false };  // Array to check if weapon is initialized
char buf[1028];
auto size = sizeof(buf);
std::string Cheat = "STREAKS";
void ErrorWindow(const char* msg, ...) {
    va_list va;
    va_start(va, msg);
    vsprintf_s(buf, size, msg, va);
    va_end(va);
    ::MessageBoxA(NULL, buf, "Error! Oh well...",
        MB_OK | MB_ICONHAND | MB_SETFOREGROUND | MB_TASKMODAL);

     raise(SIGABRT);
     _exit(3);
}

void Init() {
    // Check for ini existanse
    if (!std::filesystem::exists(PLUGIN_PATH((char*)"VCMeleeWeaponStreaks.ini"))) {
        ErrorWindow("%s: Ini file was not found! It should be at: %s!", PLUGIN_FILENAME, PLUGIN_PATH((char*)""));
    }

    // Load the ini
    CIniReader ini(PLUGIN_PATH((char*)"VCMeleeWeaponStreaks.ini"));

    Cheat = ini.ReadString("MAIN", "CheatString", "STREAKS");

    // Reading weapons params
    for (int32_t i = 0; i < 512; ++i) {
        std::string name = "WEP";
        std::string formatted_str = std::format("{}{}", name, i);
        const char* formatted_str2 = formatted_str.c_str();
    int red = ini.ReadInteger(formatted_str2, "R", -1);
    if (red != -1) { // Check if the section exists by checking a default value
        weaponInitialized[i] = true;
        r[i] = red;
        g[i] = ini.ReadInteger(formatted_str2, "G", 100);
        b[i] = ini.ReadInteger(formatted_str2, "B", 100);
        a[i] = ini.ReadInteger(formatted_str2, "A", 255);
        xstart[i] = ini.ReadFloat(formatted_str2, "X start", 0.02f);
        ystart[i] = ini.ReadFloat(formatted_str2, "Y start", 0.05f);
        zstart[i] = ini.ReadFloat(formatted_str2, "Z start", 0.07f);
        xend[i] = ini.ReadFloat(formatted_str2, "X end", 0.096f);
        yend[i] = ini.ReadFloat(formatted_str2, "Y end", -0.0175f);
        zend[i] = ini.ReadFloat(formatted_str2, "Z end", 1.096f);
         }
    }
}

void AddWeaponStreak(CPed* ped, eWeaponType WeaponType) {
    if (!weaponInitialized[WeaponType]) {
        return;  // Do not add streak if the weapon is not initialized in INI file
    }
    RwMatrix* LTM;
    static CMatrix attachMat;
    CVector start;
    CVector end;

    if (ped != NULL && ped->m_pWeaponObject != NULL) {
        LTM = RwFrameGetLTM(RpAtomicGetFrame(ped->m_pWeaponObject));
        attachMat.UpdateRW();
        attachMat = CMatrix(LTM, true);
        attachMat.UpdateRW();
        start = attachMat * CVector(xstart[WeaponType], ystart[WeaponType], zstart[WeaponType]);
        end = attachMat * CVector(xend[WeaponType], yend[WeaponType], zend[WeaponType]);
        RegStreak(uintptr_t(ped->m_pWeaponObject), r[WeaponType], g[WeaponType], b[WeaponType], a[WeaponType], start, end);
    }
}

class MeleeWeaponsStreaksSA {
public:
    MeleeWeaponsStreaksSA() {
        Events::initGameEvent += []() {
            Init();
        };
        Events::gameProcessEvent += []() {
            if (TestCheat(Cheat)) {
                Init();
                CHud::SetHelpMessage("Streaks ini file reloaded!", true, false, false);
            }
        };

        Events::reInitGameEvent += []() {
            Init();
        };

        Events::pedRenderEvent += [](CPed* ped) {
            auto weapType = ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_eWeaponType;
            if (ped && ped->m_pIntelligence) {
                if (ped->m_pIntelligence->GetTaskFighting()) {
                    AddWeaponStreak(ped, weapType);
                }
            }
        };
    }
} _MeleeWeaponsStreaksSA;
