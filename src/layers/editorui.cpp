#include <Geode/modify/EditorUI.hpp>
#include <nwo5.silly-api/include/include.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include <include.hpp>
#include "../settings.hpp"

using namespace geode::prelude;
using namespace nwo5::prelude;

class $modify(UIScalingEditorUI, EditorUI) {
    struct Fields {
        float scale = 1.0f;
    };

    static constexpr auto CUSTOM_EDGE_PADDING = 5.0f;
    static constexpr auto CUSTOM_LEFT_GROUP_GAP = 5.0f;

    static void onModify(auto& pSelf) {
        (void)pSelf.setHookPriorityPost("EditorUI::init", Priority::Late);
    }
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) {
            return false;
        }

        if (Settings::editorUIEnabled) {
            updateUIScale(Settings::editorUIScaling, Settings::editorUIVanillaPositioning);
        }
        else {
            nwo5::uiscaling::EditorUIScaleChanged().send(m_fields->scale);
        }

        return true;
    }

    void updateUIScale(float pScale, bool pVanillaPositioning) {
        auto fields = m_fields.self();

        fields->scale = pScale;

        const auto window = CCDirector::get()->getWinSize();

        const auto leftPadding = (pVanillaPositioning ? 6.0f : CUSTOM_EDGE_PADDING) * pScale;
        const auto topPadding = (pVanillaPositioning ? 0.0f : CUSTOM_EDGE_PADDING) * pScale;

        auto undoMenu = this->getChildByID("undo-menu");

        if (undoMenu) {
            Setup(undoMenu)
                .scale(pScale)
                .anchor(Anchor::Center)
                .pos(
                    ui::sw(undoMenu) / 2 + leftPadding, 
                    window.height - ui::sh(undoMenu) / 2 - topPadding
                );
        }
        
        auto settingsMenu = this->getChildByID("settings-menu");

        if (settingsMenu) {
            Setup(settingsMenu)
                .scale(pScale)
                .anchor(Anchor::Center)
                .pos(
                    window.width - (pVanillaPositioning ? 0.0f : CUSTOM_EDGE_PADDING) * pScale - ui::sw(settingsMenu) / 2, 
                    window.height - ui::sh(settingsMenu) / 2 - topPadding
                );
        }

        if (auto slider = this->getChildByID("position-slider"); slider && undoMenu && settingsMenu) {
            Setup(slider)
                .scale(pScale)
                .anchor(Anchor::Center)
                .ignoreAnchorForPos(false)
                .size(CCSizeZero)
                .pos(
                    pVanillaPositioning
                        ? ccp(
                            window.width / 2 + 30.0f * pScale,
                            window.height - 20.0f * pScale
                          )
                        : ccp(
                            Settings::editorUICenterSlider ? window.width / 2 : (ui::x(undoMenu) + ui::sw(undoMenu) / 2 + ui::x(settingsMenu) - ui::sw(settingsMenu) / 2) / 2,
                            window.height - 20.0f * pScale
                          )
                );
        }

        const auto toolbarScale = Settings::editorUIScaleToolbar ? pScale : 1.0f;
        // i have my reasons for this, of which im gatekeeping :3c
        m_toolbarHeight = (92.0f - 1.0f) * toolbarScale;
        const auto centerHeight = (window.height + m_toolbarHeight) / 2;

        if (auto editorButtons = this->getChildByID("editor-buttons-menu")) {
            const auto rightMenuScale = pScale * (Loader::get()->isModLoaded("razoom.named_editor_layers") ? 0.9f: 1.0f);

            Setup(editorButtons)
                .scale(rightMenuScale)
                .anchor(Anchor::Center)
                .pos(
                    window.width - ui::sw(editorButtons) / 2,
                    centerHeight - (pVanillaPositioning ? 2.0f : 5.0f * rightMenuScale)
                );

            if (auto layerMenu = this->getChildByID("layer-menu")) {
                Setup(layerMenu)
                    .scale(rightMenuScale)
                    .anchor(Anchor::Center)
                    .pos(
                        window.width - ui::sw(layerMenu) / 2 - (pVanillaPositioning ? 6.0f : CUSTOM_EDGE_PADDING) * rightMenuScale,
                        ui::y(editorButtons) - ui::sh(editorButtons) / 2 + (pVanillaPositioning ? 0.5f : 0.0f) * rightMenuScale
                    );

                if (m_currentLayerLabel) {
                    const auto labelWorldPos = m_currentLayerLabel->convertToWorldSpace(ui::ssize(m_currentLayerLabel) / 2);

                    if (m_layerLockSprite) {
                        Setup(m_layerLockSprite)
                            .scale(rightMenuScale * 0.5f)
                            .pos(this->convertToNodeSpace(labelWorldPos) + (m_layerLockSprite->getContentSize() / 2) * rightMenuScale);
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
                    ui::sw(playtestMenu) / 2 + leftPadding,
                    centerHeight + (pVanillaPositioning ? 2.0f : 0.0f) * pScale
                );

            if (auto playbackMenu = this->getChildByID("playback-menu")) {
                Setup(playbackMenu)
                    .scale(pScale)
                    .anchor(Anchor::Center)
                    .pos(
                        pVanillaPositioning
                            ? ccp(
                                ui::sw(playbackMenu) / 2 + leftPadding,
                                ui::y(playtestMenu) + 45.0f * pScale
                                )
                            : ccp(
                                ui::sw(playbackMenu) / 2 + leftPadding,
                                ui::y(playtestMenu) + ui::sh(playtestMenu) / 2 + CUSTOM_LEFT_GROUP_GAP * pScale + ui::sh(playbackMenu) / 2
                                )
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
                        ui::sw(zoomMenu) / 2 + (pVanillaPositioning ? 9.8f : CUSTOM_EDGE_PADDING) * pScale,
                        ui::y(playtestMenu) - ui::sh(playtestMenu) / 2 - (pVanillaPositioning ? 10.0f : CUSTOM_LEFT_GROUP_GAP) * pScale - ui::sh(zoomMenu) / 2
                    );

                if (auto linkMenu = this->getChildByID("link-menu")) {
                    Setup(linkMenu)
                        .scale(pScale)
                        .anchor(Anchor::Center)
                        .pos(
                            pVanillaPositioning
                                ? ccp(
                                    ui::x(zoomMenu) + ui::sw(zoomMenu) / 2 + ui::sw(linkMenu) / 2 + 5.0f * pScale,
                                    ui::y(playtestMenu) - ui::sh(linkMenu) / 2 + 4.0f * pScale
                                  )
                                : ccp (
                                    ui::x(zoomMenu) + ui::sw(zoomMenu) / 2 + ui::sw(linkMenu) / 2 + 10.0f * pScale,
                                    (ui::y(playtestMenu) + ui::y(zoomMenu)) / 2 - ui::sh(linkMenu) / 6
                                  )
                        );
                }
            }
        }

        if (m_objectInfoLabel) {
            Setup(m_objectInfoLabel)
                .scale(pScale * 0.6f)
                .pos((pVanillaPositioning ? 52.0f : 50.0f) * pScale, window.height - 50.f * pScale);
        }

        auto background = this->getChildByID("background-sprite");

        if (background) {
            Setup(background)
                .scaleWidthToFit(window.width)
                .scaleY(toolbarScale)
                .pos(CCPointZero)
                .anchor(Anchor::BottomLeft);
        }

        if (auto buildTabsMenu = this->getChildByID("build-tabs-menu")) {
            Setup(buildTabsMenu)
                .scale(toolbarScale)
                .anchor(Anchor::Bottom)
                .pos(window.width / 2, m_toolbarHeight - (pVanillaPositioning ? 1.0f : 0.0f) * pScale);
        }

        if (auto tabNav = this->getChildByID("alphalaneous.editortab_api/tabs-navigation-menu")) {
            Setup(tabNav)
                .scale(toolbarScale)
                .pos(window.width / 2.0f, m_toolbarHeight - 1.0f)
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
                .pos(ui::ssize(categoriesMenu) / 2 + ccp(padding, 0.0f));

            if (leftLine) {
                Setup(leftLine)
                    .scale(toolbarScale)
                    .anchor(Anchor::Center)
                    .pos(
                        ui::x(categoriesMenu) + ui::sw(categoriesMenu) / 2 + padding,
                        pVanillaPositioning ? 6.5f * toolbarScale + ui::sh(leftLine) / 2 : m_toolbarHeight / 2
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
                    window.width - ui::sw(toolbarTogglesMenu) / 2 - (pVanillaPositioning ? 3.0f * toolbarScale : toolbarPadding),
                    ui::sh(toolbarTogglesMenu) / 2
                );

            if (rightLine) { 
                Setup(rightLine)
                    .scale(toolbarScale)
                    .anchor(Anchor::Center)
                    .pos(
                        ui::x(toolbarTogglesMenu) - ui::sw(toolbarTogglesMenu) / 2 - (pVanillaPositioning ? 3.0f * toolbarScale : toolbarPadding),
                        pVanillaPositioning ? 6.5f * toolbarScale + ui::sh(rightLine) / 2 : m_toolbarHeight / 2
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

        if (auto rowMenu = this->getChildByID("razoom.object_groups/row_menu")) {
            Setup(rowMenu)
                .scale(0.55f * pScale)
                .posY(m_toolbarHeight + 20.0f * pScale);
        }

        if (auto moveMenu = this->getChildByID("hjfod.betteredit/custom-move-menu")) {
            Setup(moveMenu)
                .height(80.0f)
                .posY(toolbarPadding)
                .updateLayout();
        }

        if (m_deleteMenu) {
            Setup(m_deleteMenu)
                .scale(toolbarScale)
                .anchor(Anchor::Center)
                .ignoreAnchorForPos(false)
                .size(CCSizeZero)
                .pos(window.width / 2, m_toolbarHeight / 2);
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

        alpha::editor_tabs::updateTabMenu();

        nwo5::uiscaling::EditorUIScaleChanged().send(fields->scale);
    }
};

namespace nwo5::uiscaling {
    float getEditorUIScale() {
        if (auto ui = editor::ui<UIScalingEditorUI>()) {
            return ui->m_fields->scale;
        }
        
        return 1.0f;
    }

    void setEditorUIScale(float pScale, bool pVanillaPositioning) {
        if (auto ui = editor::ui<UIScalingEditorUI>()) {
            ui->updateUIScale(pScale, pVanillaPositioning);
        }
    }
}

$execute {
    listenForSettingChanges<bool>("editor-ui-enabled", [] (bool pEnabled) {
        nwo5::uiscaling::setEditorUIScale(pEnabled ? Settings::editorUIScaling : 1.0f, Settings::editorUIVanillaPositioning);
    });
    listenForSettingChanges<float>("editor-ui-scaling", [] (float pScale) {
        if (Settings::editorUIEnabled) {
            nwo5::uiscaling::setEditorUIScale(pScale, Settings::editorUIVanillaPositioning);
        }
    });
    listenForSettingChanges<bool>("editor-ui-vanilla-positioning", [] (bool pVanillaPositioning) {
        if (Settings::editorUIEnabled) {
            nwo5::uiscaling::setEditorUIScale(Settings::editorUIScaling, pVanillaPositioning);
        }
    });
    listenForSettingChanges<bool>("editor-ui-center-slider", [] (bool) {
        if (Settings::editorUIEnabled) {
            nwo5::uiscaling::setEditorUIScale(Settings::editorUIScaling, Settings::editorUIVanillaPositioning);
        }
    });
    listenForSettingChanges<bool>("editor-ui-scale-toolbar", [] (bool) {
        if (Settings::editorUIEnabled) {
            nwo5::uiscaling::setEditorUIScale(Settings::editorUIScaling, Settings::editorUIVanillaPositioning);
        }
    });
}