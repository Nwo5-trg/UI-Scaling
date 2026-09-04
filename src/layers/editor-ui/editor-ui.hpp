#pragma once

#include <Geode/modify/EditorUI.hpp>

class $modify(UIScalingEditorUI, EditorUI) {
    struct Fields {
        float scale = 1.0f;
    };

    static constexpr auto CUSTOM_EDGE_PADDING = 5.0f;
    static constexpr auto CUSTOM_LEFT_GROUP_GAP = 5.0f;

    static void onModify(auto& pSelf) {
        (void)pSelf.setHookPriorityPost("EditorUI::init", geode::Priority::Late);
    }

    bool init(LevelEditorLayer* editorLayer);

    void updateUIScale(float pScale, bool pVanillaPositioning, bool pScaleToolbar, bool pUseSafeArea, std::optional<float> pCustomSafeArea);
};