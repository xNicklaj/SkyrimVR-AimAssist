#include "Hooks.h"
#include "Settings.h"
#include <vector>
#include <cmath>

namespace Hooks {
    static float GetActorValueSafe(RE::Actor* a_actor, RE::ActorValue a_value) {
        if (!a_actor) return 0.0f;
        size_t offset = REL::Module::IsVR() ? 0xB8 : (REL::Module::IsAE() ? 0xB8 : 0xB0);
        auto avOwner = reinterpret_cast<RE::ActorValueOwner*>(reinterpret_cast<uintptr_t>(a_actor) + offset);
        return avOwner->GetActorValue(a_value);
    }

    struct ArrowUpdate {
    public:
        static void thunk(RE::ArrowProjectile* a_this, float a_delta) {
            if (!a_this) return func(a_this, a_delta);

            auto& runtimeData = a_this->GetProjectileRuntimeData();
            if (runtimeData.flags.any(RE::Projectile::Flags::kDestroyed)) {
                return func(a_this, a_delta);
            }

            auto shooterRef = runtimeData.shooter.get().get();
            auto player = RE::PlayerCharacter::GetSingleton();
            
            if (shooterRef != player) {
                return func(a_this, a_delta);
            }

            RE::NiPoint3 projPos = a_this->GetPosition();
            RE::NiPoint3 projVel = runtimeData.linearVelocity;
            float currentSpeed = projVel.Length();
            
            if (currentSpeed < 1.0f) {
                return func(a_this, a_delta);
            }

            auto cell = player->GetParentCell();
            if (!cell) return func(a_this, a_delta);

            std::vector<RE::Actor*> targets;
            if (auto processLists = RE::ProcessLists::GetSingleton()) {
                for (auto& handle : processLists->highActorHandles) {
                    auto actor = handle.get();
                    if (actor && actor.get() != player && !actor->IsDead() && !actor->IsPlayerTeammate()) {
                        if (actor->IsHostileToActor(player)) {
                            targets.push_back(actor.get());
                            continue;
                        }
                        auto confidence = GetActorValueSafe(actor.get(), RE::ActorValue::kConfidence);
                        if (confidence <= 0.0f) {
                            targets.push_back(actor.get());
                            continue;
                        }
                        auto aggression = GetActorValueSafe(actor.get(), RE::ActorValue::kAggression);
                        if (aggression >= 1.0f) {
                            targets.push_back(actor.get());
                            continue;
                        }
                    }
                }
            }

            if (targets.empty()) {
                if (Settings::EnableRuntimeLogs) {
                    SKSE::log::info("AimAssistVR: Arrow in flight. No valid targets found.");
                }
                return func(a_this, a_delta);
            }

            if (Settings::EnableRuntimeLogs) {
                SKSE::log::info("AimAssistVR: Arrow in flight. Found {} potential targets.", targets.size());
            }

            float archerySkill = GetActorValueSafe(player, RE::ActorValue::kArchery);
            float skillFactor = std::clamp((archerySkill - 15.0f) / (100.0f - 15.0f), 0.0f, 1.0f);
            skillFactor = skillFactor * skillFactor;

            float maxAngleCos = std::cos(Settings::MaxAngleDegrees * (3.14159265f / 180.0f));
            float currentZOffset = std::lerp(Settings::ChestZOffset, Settings::HeadZOffset, skillFactor);
            float currentMagnetism = std::lerp(Settings::MinMagnetism, Settings::MaxMagnetism, skillFactor);
            float timeScaledMagnetism = std::clamp(currentMagnetism * a_delta, 0.0f, 1.0f);

            RE::NiPoint3 projDir = projVel;
            projDir.x /= currentSpeed;
            projDir.y /= currentSpeed;
            projDir.z /= currentSpeed;

            RE::Actor* bestTarget = nullptr;
            float bestDot = -1.0f;
            RE::NiPoint3 bestTargetCenter;

            RE::Actor* absoluteBestTarget = nullptr;
            float absoluteBestDot = -1.0f;

            for (auto target : targets) {
                RE::NiPoint3 targetPos = target->GetPosition();
                targetPos.z += currentZOffset;

                RE::NiPoint3 toTarget = targetPos - projPos;
                float dist = toTarget.Length();
                if (dist < 1.0f) continue;

                toTarget.x /= dist;
                toTarget.y /= dist;
                toTarget.z /= dist;

                float dotProduct = (projDir.x * toTarget.x) + (projDir.y * toTarget.y) + (projDir.z * toTarget.z);
                
                if (dotProduct > absoluteBestDot) {
                    absoluteBestDot = dotProduct;
                    absoluteBestTarget = target;
                }

                if (dotProduct > maxAngleCos && dotProduct > bestDot) {
                    bestDot = dotProduct;
                    bestTarget = target;
                    bestTargetCenter = targetPos;
                }
            }

            if (Settings::EnableRuntimeLogs && !bestTarget && absoluteBestTarget) {
                SKSE::log::info("AimAssistVR: Target '{}' rejected (Dot: {:.4f} < MaxAngleCos: {:.4f})", 
                    absoluteBestTarget->GetName() ? absoluteBestTarget->GetName() : "Unknown", 
                    absoluteBestDot, maxAngleCos);
            }

            if (bestTarget) {
                RE::NiPoint3 toTarget = bestTargetCenter - projPos;
                float dist = toTarget.Length();
                if (dist > 0.0f) {
                    toTarget.x /= dist;
                    toTarget.y /= dist;
                    toTarget.z /= dist;

                    RE::NiPoint3 idealVel = toTarget;
                    idealVel.x *= currentSpeed;
                    idealVel.y *= currentSpeed;
                    idealVel.z *= currentSpeed;

                    runtimeData.linearVelocity.x = std::lerp(projVel.x, idealVel.x, timeScaledMagnetism);
                    runtimeData.linearVelocity.y = std::lerp(projVel.y, idealVel.y, timeScaledMagnetism);
                    runtimeData.linearVelocity.z = std::lerp(projVel.z, idealVel.z, timeScaledMagnetism);
                        
                    RE::NiPoint3 newVel = runtimeData.linearVelocity;
                    float xyLen = std::sqrt(newVel.x * newVel.x + newVel.y * newVel.y);
                        
                    a_this->data.angle.z = std::atan2(newVel.x, newVel.y);
                    a_this->data.angle.x = std::atan2(-newVel.z, xyLen);

                    if (auto node3D = a_this->Get3D()) {
                        RE::NiMatrix3 rot;
                        rot.SetEulerAnglesXYZ(a_this->data.angle.x, a_this->data.angle.y, a_this->data.angle.z);
                        node3D->local.rotate = rot;
                    }
                        
                    if (Settings::EnableRuntimeLogs) {
                        SKSE::log::info("AimAssistVR: Snapped arrow towards target '{}' (FormID: {:08X})",
                            bestTarget->GetName() ? bestTarget->GetName() : "Unknown",
                            bestTarget->GetFormID());
                    }
                }
            }

            return func(a_this, a_delta);
        }
        static inline REL::Relocation<decltype(thunk)> func;
    };

    void Install() {
        REL::Relocation<std::uintptr_t> vtable(RE::VTABLE_ArrowProjectile[0]);
        size_t updateIdx = REL::Module::IsVR() ? 0xAC : 0xAB;
        ArrowUpdate::func = vtable.write_vfunc(updateIdx, ArrowUpdate::thunk);
        SKSE::log::info("AimAssistVR: Hooked ArrowProjectile::UpdateImpl at index {:X}", updateIdx);
    }
}
