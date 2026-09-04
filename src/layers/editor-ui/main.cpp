#include <nwo5.silly-api/include/include.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include "../../settings.hpp"
#include "../include.hpp"
#include <include.hpp>
#include "include.hpp"

using namespace geode::prelude;
using namespace nwo5::prelude;

void UIScalingEditorUI::onModify(auto& pSelf) {
    (void)pSelf.setHookPriorityPost("EditorUI::init", Priority::Late);
}

bool UIScalingEditorUI::init(LevelEditorLayer* editorLayer) {
    if (!EditorUI::init(editorLayer)) {
        return false;
    }

    addOnEnterCallback([this] {
        if (Settings::editorUIEnabled) {
            this->updateUIScale(
                Settings::editorUIScaling, Settings::editorUIVanillaPositioning, Settings::editorUIScaleToolbar,
                Settings::useSafeArea, Settings::customSafeArea ? std::optional<float>{Settings::customSafeArea} : std::nullopt
            );
        }
        else {
            nwo5::uiscaling::EditorUI::Changed().send(m_fields->scale);
        }
    }, Priority::Late);

    return true;
}

void UIScalingEditorUI::updateUIScale(float pScale, bool pVanillaPositioning, bool pScaleToolbar, bool pUseSafeArea, std::optional<float> pCustomSafeArea) {
    auto fields = m_fields.self();

    fields->scale = pScale;

    const float safeOffset = pUseSafeArea ? pCustomSafeArea.value_or(utils::getSafeAreaRect().getMinX() / 2) : 0.0f;

    uiscaling::tinker::updateUI(pScale, pScaleToolbar, pUseSafeArea, pCustomSafeArea);

    const auto window = CCDirector::get()->getWinSize();

    const auto leftPadding = (pVanillaPositioning ? 6.0f : CUSTOM_EDGE_PADDING) * pScale;
    const auto topPadding = (pVanillaPositioning ? 0.0f : CUSTOM_EDGE_PADDING) * pScale;

    auto undoMenu = this->getChildByID("undo-menu");

    if (undoMenu) {
        Setup(undoMenu)
            .scale(pScale)
            .anchor(Anchor::Center)
            .pos(
                ui::sw(undoMenu) / 2 + leftPadding + safeOffset, 
                window.height - ui::sh(undoMenu) / 2 - topPadding
            );
    }
    
    auto settingsMenu = this->getChildByID("settings-menu");

    if (settingsMenu) {
        Setup(settingsMenu)
            .scale(pScale)
            .anchor(Anchor::Center)
            .pos(
                window.width - (pVanillaPositioning ? 0.0f : CUSTOM_EDGE_PADDING) * pScale - ui::sw(settingsMenu) / 2 - safeOffset, 
                window.height - ui::sh(settingsMenu) / 2 - topPadding
            );
    }

    if (auto slider = static_cast<Slider*>(this->getChildByID("position-slider")); slider && undoMenu && settingsMenu) {
        Setup(slider)
            .scale(pScale)
            .anchor(Anchor::Center)
            .ignoreAnchorForPos(false)
            .size(CCSizeZero)
            .pos(
                pVanillaPositioning
                    ? CCPoint{
                        window.width / 2 + 30.0f * pScale,
                        window.height - 20.0f * pScale
                    }
                    : CCPoint{
                        Settings::editorUICenterSlider ? window.width / 2 : (ui::x(undoMenu) + ui::sw(undoMenu) / 2 + ui::x(settingsMenu) - ui::sw(settingsMenu) / 2) / 2,
                        window.height - 20.0f * pScale
                    }
            );

        if (auto gridControls = this->getChildByID("hjfod.betteredit/grid-size-controls")) {
            const auto gridControlsScale = std::min(pScale, 0.85f) * 0.9f;
            
            const auto min = ui::x(slider) + ui::w(slider->m_sliderBar) / 2 * pScale;
            const auto max = ui::x(settingsMenu) - ui::sw(settingsMenu) / 2;
            const auto fitInbetween = max - min <= ui::w(gridControls) * gridControlsScale;

            Setup(gridControls)
                .scale(gridControlsScale)
                .anchor(fitInbetween ? Anchor::Right : Anchor::Center)
                .size(70.0f, 35.0f)
                .pos(
                    fitInbetween 
                        ? CCPoint{
                            max - 6.0f, ui::y(settingsMenu)
                        }
                        : CCPoint{
                            ui::pos(slider) - CCPoint{0.0f, ui::h(slider->m_sliderBar) / 2 * pScale - ui::sh(gridControls) / 2 }
                        }
                );
        }
    }

    const auto toolbarScale = pScaleToolbar ? pScale : 1.0f;
    // i have my reasons for this, of which im gatekeeping :3c
    // update: i forgot :fire:
    const auto toolbarHeight = (92.0f - 1.0f) * toolbarScale;
    // i should js get the node honestly remind me to change this ig
    const auto toolbarOffset = (
        uiscaling::tinker::get() 
        && uiscaling::tinker::get()->getSettingValue<bool>("StatusBar-enabled") 
        && uiscaling::tinker::get()->getSettingValue<bool>("UIScaling-enabled") 
        && pScale <= 0.9f
    )
        ? 8.0f 
        : 0.0f;
    const auto centerHeight = (window.height + toolbarHeight) / 2 + toolbarOffset / 2;

    if (auto editorButtons = this->getChildByID("editor-buttons-menu")) {
        const auto rightMenuScale = pScale * (Loader::get()->isModLoaded("razoom.named_editor_layers") ? 0.9f: 1.0f);

        Setup(editorButtons)
            .scale(rightMenuScale)
            .anchor(Anchor::Center)
            .pos(
                window.width - ui::sw(editorButtons) / 2 - safeOffset,
                centerHeight - (pVanillaPositioning ? 2.0f : 5.0f * rightMenuScale)
            );

        if (auto layerMenu = this->getChildByID("layer-menu")) {
            Setup(layerMenu)
                .scale(rightMenuScale)
                .anchor(Anchor::Center)
                .pos(
                    window.width - ui::sw(layerMenu) / 2 - (pVanillaPositioning ? 6.0f : CUSTOM_EDGE_PADDING) * rightMenuScale - safeOffset,
                    ui::y(editorButtons) - ui::sh(editorButtons) / 2 + (pVanillaPositioning ? 0.5f : 0.0f) * rightMenuScale
                );

            if (m_currentLayerLabel) {
                const auto labelWorldPos = m_currentLayerLabel->convertToWorldSpace(ui::ssize(m_currentLayerLabel) / 2);

                if (m_layerLockSprite) {
                    Setup(m_layerLockSprite)
                        .scale(rightMenuScale * 0.5f)
                        .pos(this->convertToNodeSpace(labelWorldPos) + (ui::size(m_layerLockSprite) / 2) * rightMenuScale);
                }

                if (auto lockButton = this->getChildByIDRecursive("lock-layer-button")) {
                    Setup(lockButton)
                        .scale(rightMenuScale)
                        .pos(lockButton->getParent()->convertToNodeSpace(labelWorldPos));
                }
            }
        }
    }

    if (auto playtestMenu = this->getChildByID("playtest-menu")) {
        Setup(playtestMenu)
            .scale(pScale)
            .anchor(Anchor::Center)
            .pos(
                ui::sw(playtestMenu) / 2 + leftPadding + safeOffset,
                centerHeight + (pVanillaPositioning ? 2.0f : 0.0f) * pScale
            );

        if (auto playbackMenu = this->getChildByID("playback-menu")) {
            Setup(playbackMenu)
                .scale(pScale)
                .anchor(Anchor::Center)
                .pos(
                    pVanillaPositioning
                        ? CCPoint{
                            ui::sw(playbackMenu) / 2 + leftPadding + safeOffset,
                            ui::y(playtestMenu) + 45.0f * pScale
                        }
                        : CCPoint{
                            ui::sw(playbackMenu) / 2 + leftPadding + safeOffset,
                            ui::y(playtestMenu) + ui::sh(playtestMenu) / 2 + CUSTOM_LEFT_GROUP_GAP * pScale + ui::sh(playbackMenu) / 2
                        }
                );

            if (auto startPosViewer = this->getChildByID("d050.startpositionviewer/start-pos-viewer-menu")) {
                Setup(startPosViewer)
                    .scale(pScale)
                    .pos(
                        playtestMenu->boundingBox().getMinX() + 40.0f * pScale,
                        (ui::y(playtestMenu) + ui::y(playbackMenu)) / 2
                    );
            }
        }

        if (auto zoomMenu = this->getChildByID("zoom-menu")) {
            Setup(zoomMenu)
                .scale(pScale)
                .anchor(Anchor::Center)
                .pos(
                    ui::sw(zoomMenu) / 2 + (pVanillaPositioning ? 9.8f : CUSTOM_EDGE_PADDING) * pScale + safeOffset,
                    ui::y(playtestMenu) - ui::sh(playtestMenu) / 2 - (pVanillaPositioning ? 10.0f : CUSTOM_LEFT_GROUP_GAP) * pScale - ui::sh(zoomMenu) / 2
                );

            if (auto linkMenu = this->getChildByID("link-menu")) {
                Setup(linkMenu)
                    .scale(pScale)
                    .anchor(Anchor::Center)
                    .pos(
                        pVanillaPositioning
                            ? CCPoint{
                                ui::x(zoomMenu) + ui::sw(zoomMenu) / 2 + ui::sw(linkMenu) / 2 + 5.0f * pScale,
                                ui::y(playtestMenu) - ui::sh(linkMenu) / 2 + 4.0f * pScale
                            }
                            : CCPoint{
                                ui::x(zoomMenu) + ui::sw(zoomMenu) / 2 + ui::sw(linkMenu) / 2 + 10.0f * pScale,
                                (ui::y(playtestMenu) + ui::y(zoomMenu)) / 2 - ui::sh(linkMenu) / 6
                            }
                    );
            }
        }
    }

    if (m_objectInfoLabel) {
        Setup(m_objectInfoLabel)
            .scale(pScale * 0.6f)
            .pos((pVanillaPositioning ? 52.0f : 50.0f) * pScale + safeOffset, window.height - 50.f * pScale);
    }

    auto background = this->getChildByID("background-sprite");

    if (background) {
        Setup(background)
            .scaleWidthToFit(window.width)
            .scaleY(toolbarScale)
            .pos(-1.0f, -1.0f)
            .anchor(Anchor::BottomLeft);
    }

    if (auto buildTabsMenu = this->getChildByID("build-tabs-menu")) {
        Setup(buildTabsMenu)
            .scale(toolbarScale)
            .anchor(Anchor::Bottom)
            .pos(window.width / 2, toolbarHeight - (pVanillaPositioning ? 1.0f : 0.0f) * pScale);
    }

    if (auto tabNav = this->getChildByID("alphalaneous.editortab_api/tabs-navigation-menu")) {
        Setup(tabNav)
            .scale(toolbarScale)
            .pos(window.width / 2.0f, toolbarHeight - 1.0f)
            .anchor(Anchor::Bottom);
    }

    const auto toolbarPadding = 5.0f * toolbarScale;

    auto leftLine = this->getChildByID("spacer-line-left");
    auto rightLine = this->getChildByID("spacer-line-right");

    if (auto categoriesMenu = this->getChildByID("toolbar-categories-menu")) { 
        auto padding = toolbarPadding;
        if (auto be = Loader::get()->getLoadedMod("hjfod.betteredit")) {
            if (be->getSettingValue<bool>("view-menu")) {
                padding = 0.0f;
            }
        }

        Setup(categoriesMenu)
            .scale(toolbarScale)
            .anchor(Anchor::Center)
            .pos(ui::ssize(categoriesMenu) / 2 + CCPoint{padding + safeOffset, 0.0f});

        if (leftLine) {
            Setup(leftLine)
                .scale(toolbarScale)
                .anchor(Anchor::Center)
                .pos(
                    ui::x(categoriesMenu) + ui::sw(categoriesMenu) / 2 + padding,
                    (pVanillaPositioning ? 6.5f * toolbarScale + ui::sh(leftLine) / 2 : toolbarHeight / 2)
                );
        }

        if (auto gotoMenu = this->getChildByID("razoom.object_groups/goto_obj_menu")) {
            Setup(gotoMenu)
                .scale(toolbarScale)
                .anchor(Anchor::BottomLeft)
                .pos(
                    ui::x(categoriesMenu) + ui::sw(categoriesMenu) / 2 + padding + 2.5f * toolbarScale,
                    2.5f * toolbarScale
                );
        }
    }

    if (auto toolbarTogglesMenu = this->getChildByID("toolbar-toggles-menu")) {
        Setup(toolbarTogglesMenu)
            .scale(toolbarScale)
            .anchor(Anchor::Center)
            .pos(
                window.width - ui::sw(toolbarTogglesMenu) / 2 - (pVanillaPositioning ? 3.0f * toolbarScale : toolbarPadding) - safeOffset,
                ui::sh(toolbarTogglesMenu) / 2
            );

        if (rightLine) { 
            Setup(rightLine)
                .scale(toolbarScale)
                .anchor(Anchor::Center)
                .pos(
                    ui::x(toolbarTogglesMenu) - ui::sw(toolbarTogglesMenu) / 2 - (pVanillaPositioning ? 3.0f * toolbarScale : toolbarPadding),
                    (pVanillaPositioning ? 6.5f * toolbarScale + ui::sh(rightLine) / 2 : toolbarHeight / 2)
                );
        }

        if (auto toggleMenu = this->getChildByID("razoom.object_groups/toggle_menu")) {
            Setup(toggleMenu)
                .scale(toolbarScale)
                .pos(
                    ui::x(toolbarTogglesMenu) - ui::sw(toolbarTogglesMenu) / 2 - 5.0f * toolbarScale,
                    2.5f * toolbarScale
                );
        }
    }

    if (auto moveMenu = this->getChildByID("hjfod.betteredit/custom-move-menu")) {
        Setup(moveMenu)
            .height(80.0f)
            .posY(toolbarPadding)
            .updateLayout();
    }

    if (auto rowMenu = this->getChildByID("razoom.object_groups/row_menu")) {
        Setup(rowMenu)
            .scale(0.55f * pScale)
            .posY(toolbarHeight + 20.0f * pScale);
    }

    if (m_deleteMenu) {
        Setup(m_deleteMenu)
            .scale(toolbarScale)
            .anchor(Anchor::Center)
            .ignoreAnchorForPos(false)
            .size(CCSizeZero)
            .pos(window.width / 2, toolbarHeight / 2);
    }

    this->runAction(CallFuncExt::create([=, this] {
        for (auto node : this->getChildrenExt()) {
            if (auto bar = typeinfo_cast<EditButtonBar*>(node); bar && leftLine && rightLine) {
                Setup(bar)
                    .scale(toolbarScale)
                    .anchor(Anchor::Bottom)
                    .pos((ui::x(leftLine) + ui::x(rightLine)) / 2, 0.0f);

                bar->reloadItems(
                    GameManager::get()->getIntGameVariable(GameVar::EditorButtonsPerRow),
                    GameManager::get()->getIntGameVariable(GameVar::EditorButtonRows)
                );
            }
        }
    }));

    m_toolbarHeight = toolbarHeight;

    alpha::editor_tabs::updateTabMenu();

    nwo5::uiscaling::EditorUI::Changed().send(fields->scale);
}

$execute {
    listenForSettingChanges<bool>("use-safe-area", [] (bool pUseSafeArea) {
        if (Settings::editorUIEnabled) {
            nwo5::uiscaling::EditorUI::setScale(
                Settings::editorUIScaling, Settings::editorUIVanillaPositioning, Settings::editorUIScaleToolbar,
                pUseSafeArea, Settings::customSafeArea ? std::optional<float>{Settings::customSafeArea} : std::nullopt
            );
        }
    });
    listenForSettingChanges<float>("custom-safe-area", [] (float pCustomSafeArea) {
        if (Settings::editorUIEnabled) {
            nwo5::uiscaling::EditorUI::setScale(
                Settings::editorUIScaling, Settings::editorUIVanillaPositioning, Settings::editorUIScaleToolbar,
                Settings::useSafeArea, pCustomSafeArea ? std::optional<float>{pCustomSafeArea} : std::nullopt
            );
        }
    });

    listenForSettingChanges<bool>("editor-ui-enabled", [] (bool pEnabled) {
        if (pEnabled) {
            nwo5::uiscaling::EditorUI::setScale(
                Settings::editorUIScaling, Settings::editorUIVanillaPositioning, Settings::editorUIScaleToolbar,
                Settings::useSafeArea, Settings::customSafeArea ? std::optional<float>{Settings::customSafeArea} : std::nullopt
            );
        }
        else {
            uiscaling::tinker::updateSettings();

            if (!uiscaling::tinker::scalingEnabled()) {
                nwo5::uiscaling::EditorUI::setScale(
                    1.0f, Settings::editorUIVanillaPositioning, Settings::editorUIScaleToolbar,
                    Settings::useSafeArea, Settings::customSafeArea ? std::optional<float>{Settings::customSafeArea} : std::nullopt
                );
            }
        }
    });
    listenForSettingChanges<float>("editor-ui-scaling", [] (float pScale) {
        if (Settings::editorUIEnabled) {
            nwo5::uiscaling::EditorUI::setScale(
                pScale, Settings::editorUIVanillaPositioning, Settings::editorUIScaleToolbar,
                Settings::useSafeArea, Settings::customSafeArea ? std::optional<float>{Settings::customSafeArea} : std::nullopt
            );
        }
    });
    listenForSettingChanges<bool>("editor-ui-vanilla-positioning", [] (bool pVanillaPositioning) {
        if (Settings::editorUIEnabled) {
            nwo5::uiscaling::EditorUI::setScale(
                Settings::editorUIScaling, pVanillaPositioning, Settings::editorUIScaleToolbar,
                Settings::useSafeArea, Settings::customSafeArea ? std::optional<float>{Settings::customSafeArea} : std::nullopt
            );
        }
    });
    listenForSettingChanges<bool>("editor-ui-center-slider", [] (bool) {
        if (Settings::editorUIEnabled) {
            nwo5::uiscaling::EditorUI::setScale(
                Settings::editorUIScaling, Settings::editorUIVanillaPositioning, Settings::editorUIScaleToolbar,
                Settings::useSafeArea, Settings::customSafeArea ? std::optional<float>{Settings::customSafeArea} : std::nullopt
            );
        }
    });
    listenForSettingChanges<bool>("editor-ui-scale-toolbar", [] (bool pScaleToolbar) {
        if (Settings::editorUIEnabled) {
            nwo5::uiscaling::EditorUI::setScale(
                Settings::editorUIScaling, Settings::editorUIVanillaPositioning, pScaleToolbar,
                Settings::useSafeArea, Settings::customSafeArea ? std::optional<float>{Settings::customSafeArea} : std::nullopt
            );
        }
    });
}