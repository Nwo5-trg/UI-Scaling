#include <Geode/modify/EditorUI.hpp>
#include <nwo5.silly-api/include/include.hpp>
#include "../settings.hpp"
#include <include.hpp>
#include "tinker.hpp"

using namespace geode::prelude;
using namespace nwo5::prelude;

struct UIScaleUpdated final : Event<UIScaleUpdated, bool(float, bool, bool)> {
    using Event::Event;
};

class $modify(TinkerCompatEditorUI, EditorUI) {
    struct Fields {
        bool listenToTinker = Settings::editorUIEnabled;
        bool updateTinkerSettings = true;
    };
    
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) {
            return false;
        }

        // *steals ur event*
        if (auto tinker = uiscaling::tinker::get()) {
            this->addEventListener(UIScaleUpdated(), [this, tinker] (float pScale, bool pScaleToolbar, bool) {
                auto tinker = uiscaling::tinker::get();

                if (!tinker) {
                    return ListenerResult::Propagate;
                }

                if (this->m_fields->listenToTinker) {
                    nwo5::uiscaling::EditorUI::setScale(
                        pScale, true, pScaleToolbar, tinker->getSettingValue<bool>("UIScaling-use-safe-area"),
                        tinker->getSettingValue<bool>("UIScaling-use-custom-safe-area") 
                            ? std::optional<float>{tinker->getSettingValue<float>("UIScaling-custom-safe-area")} 
                            : std::nullopt
                    );

                    return ListenerResult::Stop;
                }

                return ListenerResult::Propagate;
            }, Priority::Early);

            this->addEventListener(SettingChangedEventV3(tinker, "UIScaling-use-safe-area"), [this] (std::shared_ptr<SettingV3> pSetting) {
                return !this->m_fields->updateTinkerSettings;
            }, Priority::Early);
            this->addEventListener(SettingChangedEventV3(tinker, "UIScaling-use-custom-safe-area"), [this] (std::shared_ptr<SettingV3> pSetting) {
                return !this->m_fields->updateTinkerSettings;
            }, Priority::Early);
            this->addEventListener(SettingChangedEventV3(tinker, "UIScaling-custom-safe-area"), [this] (std::shared_ptr<SettingV3> pSetting) {
                return !this->m_fields->updateTinkerSettings;
            }, Priority::Early);
            this->addEventListener(SettingChangedEventV3(tinker, "UIScaling-scale"), [this] (std::shared_ptr<SettingV3> pSetting) {
                return !this->m_fields->updateTinkerSettings;
            }, Priority::Early);
            this->addEventListener(SettingChangedEventV3(tinker, "UIScaling-scale-toolbar"), [this] (std::shared_ptr<SettingV3> pSetting) {
                return !this->m_fields->updateTinkerSettings;
            }, Priority::Early);
        }
        
        return true;
    }
};

namespace uiscaling::tinker {
    Mod* get() {
        return Loader::get()->getLoadedMod("alphalaneous.tinker");
    }
    bool scalingEnabled() {
        return get() && get()->getSettingValue<bool>("UIScaling-enabled");
    }
    void updateUI(float pScale, bool pScaleToolbar, bool pUseSafeArea, std::optional<float> pCustomSafeArea) {
        auto ui = editor::ui<TinkerCompatEditorUI>();
        auto tinker = get();

        if (!ui || !tinker) {
            return;
        }

        auto fields = ui->m_fields.self();

        struct {
            std::shared_ptr<BoolSettingV3> safeAreaEnabledSetting = nullptr;
            bool safeAreaEnabled = true;
            std::shared_ptr<BoolSettingV3> useCustomSafeAreaSetting = nullptr;
            bool useCustomSafeArea = false;
            std::shared_ptr<FloatSettingV3> customSafeAreaSetting = nullptr;
            float customSafeArea = 24.0f;
            std::shared_ptr<FloatSettingV3> scaleSetting = nullptr;
            float scale = 1.0f;
            std::shared_ptr<BoolSettingV3> scaleToolbarSetting = nullptr;
            bool scaleToolbar = true;
        } ret;

        fields->listenToTinker = false;

        // scales ui 5 times lolll, whenever i add pause scaling ill make a "tinker state" thing that keeps track of this so i only need to change whats dirty but for now this is fine
        if ((ret.safeAreaEnabledSetting = std::static_pointer_cast<BoolSettingV3>(tinker->getSetting("UIScaling-use-safe-area")))) {
            ret.safeAreaEnabled = ret.safeAreaEnabledSetting->getValue();
            ret.safeAreaEnabledSetting->setValue(pUseSafeArea);
        }
        if ((ret.useCustomSafeAreaSetting = std::static_pointer_cast<BoolSettingV3>(tinker->getSetting("UIScaling-use-custom-safe-area")))) {
            ret.useCustomSafeArea = ret.useCustomSafeAreaSetting->getValue();
            ret.useCustomSafeAreaSetting->setValue(pCustomSafeArea.has_value());
        }
        if ((ret.customSafeAreaSetting = std::static_pointer_cast<FloatSettingV3>(tinker->getSetting("UIScaling-custom-safe-area")))) {
            ret.customSafeArea = ret.customSafeAreaSetting ->getValue();
            ret.customSafeAreaSetting ->setValue(pCustomSafeArea.value());
        }
        if ((ret.scaleSetting = std::static_pointer_cast<FloatSettingV3>(tinker->getSetting("UIScaling-scale")))) {
            ret.scale = ret.scaleSetting->getValue();
            ret.scaleSetting->setValue(pScale);
        }
        if ((ret.scaleToolbarSetting = std::static_pointer_cast<BoolSettingV3>(tinker->getSetting("UIScaling-scale-toolbar")))) {
            ret.scaleToolbar = ret.scaleToolbarSetting->getValue();
            ret.scaleToolbarSetting->setValue(pScaleToolbar);
        }

        fields->listenToTinker = Settings::editorUIEnabled;

        fields->updateTinkerSettings = false;

        if (ret.safeAreaEnabledSetting) {
            ret.safeAreaEnabledSetting->setValue(ret.safeAreaEnabled);
        }
        if (ret.useCustomSafeAreaSetting) {
            ret.useCustomSafeAreaSetting->setValue(ret.useCustomSafeArea);
        }
        if (ret.customSafeAreaSetting) {
            ret.customSafeAreaSetting->setValue(ret.customSafeArea);
        }
        if (ret.scaleSetting) {
            ret.scaleSetting->setValue(ret.scale);
        }
        if (ret.scaleToolbarSetting) {
            ret.scaleToolbarSetting->setValue(ret.scaleToolbar);
        }

        fields->updateTinkerSettings = true;
    }
    void updateSettings() {
        if (auto tinker = get()) {
            if (auto setting = std::static_pointer_cast<BoolSettingV3>(tinker->getSetting("UIScaling-use-safe-area"))) {
                setting->setValue(setting->getValue());
            }
            if (auto setting = std::static_pointer_cast<BoolSettingV3>(tinker->getSetting("UIScaling-use-custom-safe-area"))) {
                setting->setValue(setting->getValue());
            }
            if (auto setting = std::static_pointer_cast<FloatSettingV3>(tinker->getSetting("UIScaling-custom-safe-area"))) {
                setting->setValue(setting->getValue());
            }
            if (auto setting = std::static_pointer_cast<FloatSettingV3>(tinker->getSetting("UIScaling-scale"))) {
                setting->setValue(setting->getValue());
            }
            if (auto setting = std::static_pointer_cast<FloatSettingV3>(tinker->getSetting("UIScaling-scale-toolbar"))) {
                setting->setValue(setting->getValue());
            }
        }
    }
}

$execute {
    listenForSettingChanges<bool>("editor-ui-enabled", [] (bool pEnabled) {
        if (auto ui = editor::ui<TinkerCompatEditorUI>()) {
            ui->m_fields->listenToTinker = pEnabled;
        }
    });
}