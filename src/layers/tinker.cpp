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

namespace {
    template<typename T>
    requires std::same_as<bool, T> || std::same_as<float, T>
    struct TinkerSetting {
        std::shared_ptr<typename SettingTypeForValueType<T>::SettingType> setting = nullptr;
        T value = T{};
        T realValue = T{};
        bool dirty = false;

        void set(T pVal) {
            realValue = setting->getValue();
            setting->setValue(pVal);
            value = pVal;
            dirty = true;
        }
        bool unsynced() {
            return value != realValue;
        }
        void sync() {
           value = realValue;
           setting->setValue(setting->getValue()); 
        }
        void restore() {
            setting->setValue(realValue);
            dirty = false;
        }
    };

    auto& tinkerState() {
        static struct {
            TinkerSetting<bool> safeAreaEnabled;
            TinkerSetting<bool> useCustomSafeArea;
            TinkerSetting<float> customSafeArea;
            TinkerSetting<float> scale;
            TinkerSetting<bool> scaleToolbar;
        } val;
        return val;
    }
}

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
                if (!tinker) {
                    return ListenerResult::Propagate;
                }

                if (this->m_fields->listenToTinker && !Settings::preferTinkerScaling) {
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
    void updateUI(float pScale, bool pScaleToolbar, bool pUseSafeArea, std::optional<float> pCustomSafeArea) {
        auto ui = editor::ui<TinkerCompatEditorUI>();
        auto tinker = get();

        if (!ui || !tinker) {
            return;
        }

        auto fields = ui->m_fields.self();
        auto& state = tinkerState();

        fields->listenToTinker = false;

        if (auto& setting = state.safeAreaEnabled.setting; !setting || state.safeAreaEnabled.value != pUseSafeArea) {
            if (!setting) {
                setting = std::static_pointer_cast<BoolSettingV3>(tinker->getSetting("UIScaling-use-safe-area"));
            }
        
            state.safeAreaEnabled.set(pUseSafeArea);
        }
        if (auto& setting = state.useCustomSafeArea.setting; !setting || state.useCustomSafeArea.value != pCustomSafeArea.has_value()) {
            if (!setting) {
                setting = std::static_pointer_cast<BoolSettingV3>(tinker->getSetting("UIScaling-use-custom-safe-area"));
            }
        
            state.useCustomSafeArea.set(pCustomSafeArea.has_value());
        }
        if (auto& setting = state.customSafeArea.setting; pCustomSafeArea.has_value() && (!setting || state.customSafeArea.value != pCustomSafeArea.value())) {
            if (!setting) {
                setting = std::static_pointer_cast<FloatSettingV3>(tinker->getSetting("UIScaling-custom-safe-area"));
            }
        
            state.customSafeArea.set(pCustomSafeArea.value());
        }
        if (auto& setting = state.scale.setting; !setting || state.scale.value != pScale) {
            if (!setting) {
                setting = std::static_pointer_cast<FloatSettingV3>(tinker->getSetting("UIScaling-scale"));
            }
        
            state.scale.set(pScale);
        }
        if (auto& setting = state.scaleToolbar.setting; !setting || state.scaleToolbar.value != pScaleToolbar) {
            if (!setting) {
                setting = std::static_pointer_cast<BoolSettingV3>(tinker->getSetting("UIScaling-scale-toolbar"));
            }
        
            state.scaleToolbar.set(pScaleToolbar);
        }

        fields->listenToTinker = Settings::editorUIEnabled;

        fields->updateTinkerSettings = false;

        if (state.safeAreaEnabled.dirty) {
            state.safeAreaEnabled.restore();
        }
        if (state.useCustomSafeArea.dirty) {
            state.useCustomSafeArea.restore();
        }
        if (state.customSafeArea.dirty) {
            state.customSafeArea.restore();
        }
        if (state.scale.dirty) {
            state.scale.restore();
        }
        if (state.scaleToolbar.dirty) {
            state.scaleToolbar.restore();
        }

        fields->updateTinkerSettings = true;
    }
    void updateSettings() {
        if (auto tinker = get()) {
            auto& state = tinkerState();
            
            if (state.safeAreaEnabled.setting && state.safeAreaEnabled.unsynced()) {
                state.safeAreaEnabled.sync();
            }
            if (state.useCustomSafeArea.setting && state.useCustomSafeArea.unsynced()) {
                state.useCustomSafeArea.sync();
            }
            if (state.customSafeArea.setting && state.customSafeArea.unsynced()) {
                state.customSafeArea.sync();
            }
            if (state.scaleToolbar.setting && state.scaleToolbar.unsynced()) {
                state.scaleToolbar.sync();
            }

            // update once even if nothing is dirty so prefer tinker positioning can update properly
            if (state.scale.setting) {
                state.scale.sync();
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