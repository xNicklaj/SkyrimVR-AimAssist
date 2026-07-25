#pragma once

struct Settings {
    static inline float BaseRadius = 80.0f;
    static inline float MaxSkillRadiusExpansion = 100.0f;
    static inline float CloseMagnetism = 12.0f;
    static inline float FarMagnetism = 0.5f;
    static inline float CloseDistance = 500.0f;
    static inline float FarDistance = 3000.0f;
    static inline float ChestZOffset = 50.0f;
    static inline float HeadZOffset = 120.0f;
    static inline bool EnableRuntimeLogs = true;
    static inline bool TargetNeutralAnimals = true;
    static inline bool TargetNeutralNPCs = false;

    static void Load();
    static void Save();
    static void __stdcall RenderMenu();
};
