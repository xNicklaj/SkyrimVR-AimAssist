#pragma once

struct Settings {
    static inline float MaxAngleDegrees = 15.0f;
    static inline float MinMagnetism = 3.0f;
    static inline float MaxMagnetism = 12.0f;
    static inline float ChestZOffset = 50.0f;
    static inline float HeadZOffset = 120.0f;
    static inline bool EnableRuntimeLogs = false;
    static inline bool TargetNeutralAnimals = true;
    static inline bool TargetNeutralNPCs = false;

    static void Load();
};
