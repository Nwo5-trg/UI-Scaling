#pragma once

#include "editor-ui.hpp"

namespace tinker::api::ui_scaling {
    struct UIScaleUpdated final : geode::Event<UIScaleUpdated, bool(float, bool, bool)> {
        using Event::Event;
    };
}

namespace nwo5::uiscaling {
    namespace compat {
        enum class ScalingSource {
            Other = 0,
            UIScaling = 1,
            Tinker = 2
        };
    
        namespace EditorUI {
            /// event sent every time editor ui scale changed, and on editor init even if ui scaling is disabled
            /// @note runs even if ui scaling mod isnt installed
            /// @note runs after ui is scaled and positioned
            struct Changed final : geode::GlobalEvent<Changed, bool(std::string_view pID, float pScale, ScalingSource pMod), bool(float pScale, ScalingSource pMod), std::string> {
                using GlobalEvent::GlobalEvent;

                Changed(std::string pID = geode::Mod::get()->getID())
                    : GlobalEvent(std::move(pID)) {}
            };
        }
    }
    
    namespace impl {
        inline bool shouldListenToUIScaleAPI() {
            static auto mod = geode::Loader::get()->getLoadedMod("nwo5.ui-scaling");
            static auto tinker = geode::Loader::get()->getLoadedMod("alphalaneous.tinker");

            if (!mod) {
                return false;
            }

            if (!tinker || (mod->getSettingValue<bool>("editor-ui-enabled") && !mod->getSettingValue<bool>("prefer-tinker-enabled"))) {
                return true;
            }

            return false;
        }
        inline bool shouldListenToTinker() {
            static auto mod = geode::Loader::get()->getLoadedMod("alphalaneous.tinker");

            return mod;
        }

        inline auto uiscalingListener = EditorUI::Changed().listen([] (float pScale) {
            static const auto id = geode::Mod::get()->getID();

            if (shouldListenToUIScaleAPI()) {
                compat::EditorUI::Changed(id).send(pScale, compat::ScalingSource::UIScaling);
            }
        }, geode::Priority::Late);
        inline auto tinkerListener = tinker::api::ui_scaling::UIScaleUpdated().listen([] (float pScale, bool, bool) {
            static const auto id = geode::Mod::get()->getID();

            if (shouldListenToTinker() && !shouldListenToUIScaleAPI()) {
                compat::EditorUI::Changed(id).send(pScale, compat::ScalingSource::Tinker);
            }
        }, geode::Priority::Late);
        inline auto vanillaListener = geode::SceneEvent().listen([] (cocos2d::CCScene* pScene) {
            static const auto id = geode::Mod::get()->getID();
            static auto be = geode::Loader::get()->getLoadedMod("hjfod.betteredit");

            if (!shouldListenToUIScaleAPI() && !shouldListenToTinker() && pScene && pScene->getChildByType<LevelEditorLayer>()) {
                compat::EditorUI::Changed(id).send(be ? be->getSettingValue<bool>("scale-factor") : 1.0f, compat::ScalingSource::Other);
            }
        });
    }

    namespace prelude {
        namespace uiscaling = nwo5::uiscaling;

        using compat::ScalingSource;
    }
}