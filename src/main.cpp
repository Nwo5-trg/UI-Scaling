#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/EditorOptionsLayer.hpp>
#include <nwo5.silly-api/include/ui/include.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/ui/Popup.hpp>
#include "settings.hpp"

using namespace geode::prelude;
using namespace nwo5::ui::prelude;

class $modify(SettingsEditorPauseLayer, EditorPauseLayer) {
    bool init(LevelEditorLayer* layer) {
        if (!EditorPauseLayer::init(layer)) {
            return false;
        }

        auto guidelinesMenu = this->getChildByID("guidelines-menu");

        if (!guidelinesMenu || !Settings::settingsButtonInEditor.get()) {
            return true;
        }

        guidelinesMenu->addChild(Setup(ui::circleButtonFrame(
            "edit_areaModeBtn05_001.png", CircleBaseColor::Green, this, menu_selector(SettingsEditorPauseLayer::onSettingsButton), 0.75f
        ))
            .scaleToFit(guidelinesMenu->getChildByID("help-button")->getScaledContentWidth())
        );
        guidelinesMenu->updateLayout();
        
        return true;
    }

    void onSettingsButton(CCObject*) {
        if (auto popup = geode::openSettingsPopup(Mod::get(), true)) {
            popup->setScale(0.65f);
            popup->setOpacity(0);

            onResume(nullptr);
        }
    }
};

class $modify(EditorOptionsLayer) {
    void onButtonRows(CCObject* sender) {
        if (sender->getTag()) {
            m_buttonRows++;
        }
        else if (m_buttonRows > 1) {
            m_buttonRows--;
        }

        if (auto label = m_buttonRowsLabel) {
            label->setString(fmt::to_string(m_buttonRows).c_str());
        }
    }

    void onButtonsPerRow(CCObject* sender) {
        if (sender->getTag()) {
            m_buttonsPerRow++;
        }
        else if (m_buttonsPerRow > 1) {
            m_buttonsPerRow--;
        }

        if (auto label = m_buttonsPerRowLabel) {
            label->setString(fmt::to_string(m_buttonsPerRow).c_str());
        }
    }
};

$on_mod(Loaded) {
    SettingsManager::get()->load();
}