#include "Settings.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "SimpleIni.h"

void Settings::Load() {
    CSimpleIniA ini;
    ini.SetUnicode();
    auto err = ini.LoadFile(R"(Data\SKSE\Plugins\AimAssistVR.ini)");
    if (err < 0) {
        // Save defaults
        ini.SetDoubleValue("Settings", "MaxAngleDegrees", MaxAngleDegrees);
        ini.SetDoubleValue("Settings", "MinMagnetism", MinMagnetism);
        ini.SetDoubleValue("Settings", "MaxMagnetism", MaxMagnetism);
        ini.SetDoubleValue("Settings", "ChestZOffset", ChestZOffset);
        ini.SetDoubleValue("Settings", "HeadZOffset", HeadZOffset);
        ini.SetBoolValue("Settings", "EnableRuntimeLogs", EnableRuntimeLogs);
        ini.SaveFile(R"(Data\SKSE\Plugins\AimAssistVR.ini)");
    } else {
        MaxAngleDegrees = static_cast<float>(ini.GetDoubleValue("Settings", "MaxAngleDegrees", MaxAngleDegrees));
        MinMagnetism = static_cast<float>(ini.GetDoubleValue("Settings", "MinMagnetism", MinMagnetism));
        MaxMagnetism = static_cast<float>(ini.GetDoubleValue("Settings", "MaxMagnetism", MaxMagnetism));
        ChestZOffset = static_cast<float>(ini.GetDoubleValue("Settings", "ChestZOffset", ChestZOffset));
        HeadZOffset = static_cast<float>(ini.GetDoubleValue("Settings", "HeadZOffset", HeadZOffset));
        EnableRuntimeLogs = ini.GetBoolValue("Settings", "EnableRuntimeLogs", EnableRuntimeLogs);
    }
}
