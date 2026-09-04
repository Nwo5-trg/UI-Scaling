#pragma once

namespace uiscaling::tinker {
    geode::Mod* get();
    bool scalingEnabled();
    /// trick tinker into making this the ui scale
    void updateUI(float pScale, bool pScaleToolbar, bool pUseSafeArea, std::optional<float> pCustomSafeArea);
    /// fix tinker state
    void updateSettings();
}