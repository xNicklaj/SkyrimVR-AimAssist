#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include "Settings.h"
#include <SKSE/SKSE.h>
#include <RE/Skyrim.h>
#include "SimpleIni.h"
#include <SKSEMenuFramework.h>

void Settings::Save() {
    CSimpleIniA ini;
    ini.SetUnicode();
    ini.LoadFile(R"(Data\SKSE\Plugins\AimAssistVR.ini)");
    
    ini.SetDoubleValue("Main", "BaseRadius", BaseRadius);
    ini.SetDoubleValue("Main", "MaxSkillRadiusExpansion", MaxSkillRadiusExpansion);
    ini.SetDoubleValue("Main", "CloseMagnetism", CloseMagnetism);
    ini.SetDoubleValue("Main", "FarMagnetism", FarMagnetism);
    ini.SetDoubleValue("Main", "CloseDistance", CloseDistance);
    ini.SetDoubleValue("Main", "FarDistance", FarDistance);
    ini.SetDoubleValue("Main", "ChestZOffset", ChestZOffset);
    ini.SetDoubleValue("Main", "HeadZOffset", HeadZOffset);
    ini.SetBoolValue("Main", "EnableRuntimeLogs", EnableRuntimeLogs);
    ini.SetBoolValue("Main", "TargetNeutralAnimals", TargetNeutralAnimals);
    ini.SetBoolValue("Main", "TargetNeutralNPCs", TargetNeutralNPCs);
    
    ini.SaveFile(R"(Data\SKSE\Plugins\AimAssistVR.ini)");
}

void __stdcall Settings::RenderMenu() {
    bool changed = false;
    
    if (ImGuiMCP::CollapsingHeader("Targeting Geometry", ImGuiMCP::ImGuiTreeNodeFlags_DefaultOpen)) {
        changed |= ImGuiMCP::SliderFloat("Base Radius", &BaseRadius, 10.0f, 200.0f);
        changed |= ImGuiMCP::SliderFloat("Max Skill Radius Expansion", &MaxSkillRadiusExpansion, 0.0f, 500.0f);
        changed |= ImGuiMCP::SliderFloat("Chest Z Offset", &ChestZOffset, -100.0f, 100.0f);
        changed |= ImGuiMCP::SliderFloat("Head Z Offset", &HeadZOffset, -100.0f, 200.0f);
    }
    
    if (ImGuiMCP::CollapsingHeader("Magnetism Strength", ImGuiMCP::ImGuiTreeNodeFlags_DefaultOpen)) {
        changed |= ImGuiMCP::SliderFloat("Close Magnetism", &CloseMagnetism, 0.1f, 20.0f);
        changed |= ImGuiMCP::SliderFloat("Far Magnetism", &FarMagnetism, 0.1f, 10.0f);
    }
    
    if (ImGuiMCP::CollapsingHeader("Distances", ImGuiMCP::ImGuiTreeNodeFlags_DefaultOpen)) {
        changed |= ImGuiMCP::SliderFloat("Close Distance", &CloseDistance, 100.0f, 2000.0f);
        changed |= ImGuiMCP::SliderFloat("Far Distance", &FarDistance, 1000.0f, 10000.0f);
    }
    
    if (ImGuiMCP::CollapsingHeader("General", ImGuiMCP::ImGuiTreeNodeFlags_DefaultOpen)) {
        changed |= ImGuiMCP::Checkbox("Enable Runtime Logs", &EnableRuntimeLogs);
        changed |= ImGuiMCP::Checkbox("Target Neutral Animals", &TargetNeutralAnimals);
        changed |= ImGuiMCP::Checkbox("Target Neutral NPCs", &TargetNeutralNPCs);
    }
    
    if (changed) {
        Save();
    }
}

void Settings::Load() {
    CSimpleIniA ini;
    ini.SetUnicode();
    auto err = ini.LoadFile(R"(Data\SKSE\Plugins\AimAssistVR.ini)");
    if (err < 0) {
        // Save defaults
        ini.SetDoubleValue("Main", "BaseRadius", BaseRadius);
        ini.SetDoubleValue("Main", "MaxSkillRadiusExpansion", MaxSkillRadiusExpansion);
        ini.SetDoubleValue("Main", "CloseMagnetism", CloseMagnetism);
        ini.SetDoubleValue("Main", "FarMagnetism", FarMagnetism);
        ini.SetDoubleValue("Main", "CloseDistance", CloseDistance);
        ini.SetDoubleValue("Main", "FarDistance", FarDistance);
        ini.SetDoubleValue("Main", "ChestZOffset", ChestZOffset);
        ini.SetDoubleValue("Main", "HeadZOffset", HeadZOffset);
        ini.SetBoolValue("Main", "EnableRuntimeLogs", EnableRuntimeLogs);
        ini.SetBoolValue("Main", "TargetNeutralAnimals", TargetNeutralAnimals);
        ini.SetBoolValue("Main", "TargetNeutralNPCs", TargetNeutralNPCs);
        ini.SaveFile(R"(Data\SKSE\Plugins\AimAssistVR.ini)");
    } else {
        BaseRadius = static_cast<float>(ini.GetDoubleValue("Main", "BaseRadius", BaseRadius));
        MaxSkillRadiusExpansion = static_cast<float>(ini.GetDoubleValue("Main", "MaxSkillRadiusExpansion", MaxSkillRadiusExpansion));
        CloseMagnetism = static_cast<float>(ini.GetDoubleValue("Main", "CloseMagnetism", CloseMagnetism));
        FarMagnetism = static_cast<float>(ini.GetDoubleValue("Main", "FarMagnetism", FarMagnetism));
        CloseDistance = static_cast<float>(ini.GetDoubleValue("Main", "CloseDistance", CloseDistance));
        FarDistance = static_cast<float>(ini.GetDoubleValue("Main", "FarDistance", FarDistance));
        ChestZOffset = static_cast<float>(ini.GetDoubleValue("Main", "ChestZOffset", ChestZOffset));
        HeadZOffset = static_cast<float>(ini.GetDoubleValue("Main", "HeadZOffset", HeadZOffset));
        EnableRuntimeLogs = ini.GetBoolValue("Main", "EnableRuntimeLogs", EnableRuntimeLogs);
        TargetNeutralAnimals = ini.GetBoolValue("Main", "TargetNeutralAnimals", TargetNeutralAnimals);
        TargetNeutralNPCs = ini.GetBoolValue("Main", "TargetNeutralNPCs", TargetNeutralNPCs);
    }

    SKSE::log::info("AimAssistVR Settings Loaded:");
    SKSE::log::info("  BaseRadius: {}", BaseRadius);
    SKSE::log::info("  MaxSkillRadiusExpansion: {}", MaxSkillRadiusExpansion);
    SKSE::log::info("  CloseMagnetism: {}", CloseMagnetism);
    SKSE::log::info("  FarMagnetism: {}", FarMagnetism);
    SKSE::log::info("  CloseDistance: {}", CloseDistance);
    SKSE::log::info("  FarDistance: {}", FarDistance);
    SKSE::log::info("  ChestZOffset: {}", ChestZOffset);
    SKSE::log::info("  HeadZOffset: {}", HeadZOffset);
    SKSE::log::info("  EnableRuntimeLogs: {}", EnableRuntimeLogs);
    SKSE::log::info("  TargetNeutralAnimals: {}", TargetNeutralAnimals);
    SKSE::log::info("  TargetNeutralNPCs: {}", TargetNeutralNPCs);
}
