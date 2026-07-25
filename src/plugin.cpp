#include <SKSE/SKSE.h>
#include <RE/Skyrim.h>
#include "Hooks.h"
#include "Settings.h"
#include <spdlog/sinks/basic_file_sink.h>

void InitializeLogging() {
    auto path = SKSE::log::log_directory();
    if (!path) {
        SKSE::stl::report_and_fail("Failed to find standard logging directory");
    }

    *path /= "AimAssistVR.log";
    auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

    auto log = std::make_shared<spdlog::logger>("global log", sink);

    log->set_level(spdlog::level::info);
    log->flush_on(spdlog::level::info);

    spdlog::set_default_logger(std::move(log));
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
}

void OnDataLoaded() {
    Hooks::Install();
    SKSE::log::info("AimAssistVR hooks installed.");
}

void MessageHandler(SKSE::MessagingInterface::Message* a_msg) {
    switch (a_msg->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            OnDataLoaded();
            break;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
    InitializeLogging();
    SKSE::log::info("AimAssistVR plugin loading...");

    SKSE::Init(a_skse);

    Settings::Load();
    SKSE::log::info("AimAssistVR settings loaded.");

    auto messaging = SKSE::GetMessagingInterface();
    if (messaging->RegisterListener(MessageHandler)) {
        SKSE::log::info("Messaging interface registered.");
    } else {
        SKSE::log::critical("Failed to register messaging interface.");
        return false;
    }

    SKSE::log::info("AimAssistVR plugin loaded successfully.");
    return true;
}
