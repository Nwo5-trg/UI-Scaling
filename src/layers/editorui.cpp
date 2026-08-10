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
                .anchor(CENTER_ANCHOR)
                .pos(
                    undoMenu->getScaledContentWidth() / 2 + leftPadding, 
                    window.height - undoMenu->getScaledContentHeight() / 2 - topPadding
                );
        }
        
        auto settingsMenu = this->getChildByID("settings-menu");

        if (settingsMenu) {
            Setup(settingsMenu)
                .scale(pScale)
                .anchor(CENTER_ANCHOR)
                .pos(
                    window.width - (pVanillaPositioning ? 0.0f : CUSTOM_EDGE_PADDING) * pScale - settingsMenu->getScaledContentWidth() / 2, 
                    window.height - settingsMenu->getScaledContentHeight() / 2 - topPadding
                );
        }

        if (auto slider = this->getChildByID("position-slider"); slider && undoMenu && settingsMenu) {
            Setup(slider)
                .scale(pScale)
                .anchor(CENTER_ANCHOR)
                .ignoreAnchorForPos(false)
                .size(CCSizeZero)
                .pos(
                    pVanillaPositioning
                        ? ccp(
                            window.width / 2 + 30.0f * pScale,
                            window.height - 20.0f * pScale
                          )
                        : ccp(
                            Settings::editorUICenterSlider ? window.width / 2 : (undoMenu->getPositionX() + undoMenu->getScaledContentWidth() / 2 + settingsMenu->getPositionX() - settingsMenu->getScaledContentWidth() / 2) / 2,
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
                .anchor(CENTER_ANCHOR)
                .pos(
                    window.width - editorButtons->getScaledContentWidth() / 2,
                    centerHeight - (pVanillaPositioning ? 2.0f : 5.0f * rightMenuScale)
                );

            if (auto layerMenu = this->getChildByID("layer-menu")) {
                Setup(layerMenu)
                    .scale(rightMenuScale)
                    .anchor(CENTER_ANCHOR)
                    .pos(
                        window.width - layerMenu->getScaledContentWidth() / 2 - (pVanillaPositioning ? 6.0f : CUSTOM_EDGE_PADDING) * rightMenuScale,
                        editorButtons->getPositionY() - editorButtons->getScaledContentHeight() / 2 + (pVanillaPositioning ? 0.5f : 0.0f) * rightMenuScale
                    );

                if (m_currentLayerLabel) {
                    const auto labelWorldPos = m_currentLayerLabel->convertToWorldSpace(m_currentLayerLabel->getScaledContentSize() / 2);

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
                .anchor(CENTER_ANCHOR)
                .pos(
                    playtestMenu->getScaledContentWidth() / 2 + leftPadding,
                    centerHeight + (pVanillaPositioning ? 2.0f : 0.0f) * pScale
                );

            if (auto playbackMenu = this->getChildByID("playback-menu")) {
                Setup(playbackMenu)
                    .scale(pScale)
                    .anchor(CENTER_ANCHOR)
                    .pos(
                        pVanillaPositioning
                            ? ccp(
                                playbackMenu->getScaledContentWidth() / 2 + leftPadding,
                                playtestMenu->getPositionY() + 45.0f * pScale
                                )
                            : ccp(
                                playbackMenu->getScaledContentWidth() / 2 + leftPadding,
                                playtestMenu->getPositionY() + playtestMenu->getScaledContentHeight() / 2 + CUSTOM_LEFT_GROUP_GAP * pScale + playbackMenu->getScaledContentHeight() / 2
                                )
                    );

                if (auto startPosViewer = this->getChildByID("d050.startpositionviewer/start-pos-viewer-menu")) {
                    Setup(startPosViewer)
                        .scale(pScale)
                        .pos(
                            playtestMenu->boundingBox().getMinX() + 40.0f * pScale,
                            (playtestMenu->getPositionY() + playbackMenu->getPositionY()) / 2
                        );
                }
            }

            if (auto zoomMenu = this->getChildByID("zoom-menu")) {
                Setup(zoomMenu)
                    .scale(pScale)
                    .anchor(CENTER_ANCHOR)
                    .pos(
                        zoomMenu->getScaledContentWidth() / 2 + (pVanillaPositioning ? 9.8f : CUSTOM_EDGE_PADDING) * pScale,
                        playtestMenu->getPositionY() - playtestMenu->getScaledContentHeight() / 2 - (pVanillaPositioning ? 10.0f : CUSTOM_LEFT_GROUP_GAP) * pScale - zoomMenu->getScaledContentHeight() / 2
                    );

                if (auto linkMenu = this->getChildByID("link-menu")) {
                    Setup(linkMenu)
                        .scale(pScale)
                        .anchor(CENTER_ANCHOR)
                        .pos(
                            pVanillaPositioning
                                ? ccp(
                                    zoomMenu->getPositionX() + zoomMenu->getScaledContentWidth() / 2 + linkMenu->getScaledContentWidth() / 2 + 5.0f * pScale,
                                    playtestMenu->getPositionY() - linkMenu->getScaledContentHeight() / 2 + 4.0f * pScale
                                  )
                                : ccp (
                                    zoomMenu->getPositionX() + zoomMenu->getScaledContentWidth() / 2 + linkMenu->getScaledContentWidth() / 2 + 10.0f * pScale,
                                    (playtestMenu->getPositionY() + zoomMenu->getPositionY()) / 2 - linkMenu->getScaledContentHeight() / 6
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
                .anchor(BOTTOM_LEFT_ANCHOR);
        }

        if (auto buildTabsMenu = this->getChildByID("build-tabs-menu")) {
            Setup(buildTabsMenu)
                .scale(toolbarScale)
                .anchor(BOTTOM_CENTER_ANCHOR)
                .pos(window.width / 2, m_toolbarHeight - (pVanillaPositioning ? 1.0f : 0.0f) * pScale);
        }

        if (auto tabNav = this->getChildByID("alphalaneous.editortab_api/tabs-navigation-menu")) {
            tabNav->setScale(toolbarScale);
            tabNav->setPosition({window.width / 2.f, m_toolbarHeight - 1.f});
            tabNav->setAnchorPoint({0.5f, 0.f});
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
                .anchor(CENTER_ANCHOR)
                .pos(categoriesMenu->getScaledContentSize() / 2 + ccp(padding, 0.0f));

            if (leftLine) {
                Setup(leftLine)
                    .scale(toolbarScale)
                    .anchor(CENTER_ANCHOR)
                    .pos(
                        categoriesMenu->getPositionX() + categoriesMenu->getScaledContentWidth() / 2 + padding,
                        pVanillaPositioning ? 6.5f * toolbarScale + leftLine->getScaledContentHeight() / 2 : m_toolbarHeight / 2
                    );
            }

            if (auto gotoMenu = this->getChildByID("razoom.object_groups/goto_obj_menu")) {
                Setup(gotoMenu)
                    .scale(toolbarScale)
                    .anchor(BOTTOM_LEFT_ANCHOR)
                    .pos(
                        categoriesMenu->getPositionX() + categoriesMenu->getScaledContentWidth() / 2 + padding + 2.5f * toolbarScale,
                        2.5f * toolbarScale
                    );
            }
        }

        if (auto toolbarTogglesMenu = this->getChildByID("toolbar-toggles-menu")) {
            Setup(toolbarTogglesMenu)
                .scale(toolbarScale)
                .anchor(CENTER_ANCHOR)
                .pos(
                    window.width - toolbarTogglesMenu->getScaledContentWidth() / 2 - (pVanillaPositioning ? 3.0f * toolbarScale : toolbarPadding),
                    toolbarTogglesMenu->getScaledContentHeight() / 2
                );

            if (rightLine) { 
                Setup(rightLine)
                    .scale(toolbarScale)
                    .anchor(CENTER_ANCHOR)
                    .pos(
                        toolbarTogglesMenu->getPositionX() - toolbarTogglesMenu->getScaledContentWidth() / 2 - (pVanillaPositioning ? 3.0f * toolbarScale : toolbarPadding),
                        pVanillaPositioning ? 6.5f * toolbarScale + rightLine->getScaledContentHeight() / 2 : m_toolbarHeight / 2
                    );
            }

            if (auto toggleMenu = this->getChildByID("razoom.object_groups/toggle_menu")) {
                Setup(toggleMenu)
                    .scale(toolbarScale)
                    .pos(
                        toolbarTogglesMenu->getPositionX() - toolbarTogglesMenu->getScaledContentWidth() / 2 - 5.0f * toolbarScale,
                        2.5f * toolbarScale
                    );
            }
        }

        if (auto rowMenu = this->getChildByID("razoom.object_groups/row_menu")) {
            rowMenu->setScale(0.55f * pScale);
            rowMenu->setPositionY(m_toolbarHeight + 20.0f * pScale);
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
                .anchor(CENTER_ANCHOR)
                .ignoreAnchorForPos(false)
                .size(CCSizeZero)
                .pos(window.width / 2, m_toolbarHeight / 2);
        }
 
        this->runAction(CallFuncExt::create([=, this] {
            for (auto node : this->getChildrenExt()) {
                if (auto bar = typeinfo_cast<EditButtonBar*>(node); bar && leftLine && rightLine) {
                    Setup(bar)
                        .scale(toolbarScale)
                        .anchor(BOTTOM_CENTER_ANCHOR)
                        .pos((leftLine->getPositionX() + rightLine->getPositionX()) / 2, 0.0f);

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