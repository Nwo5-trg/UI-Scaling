#pragma once

#include "export.hpp"

namespace nwo5::uiscaling {
    /// event sent every time editor ui scale changed, and on editor init even if ui scaling is disabled
    /// @note runs after ui is scaled and positioned
    struct EditorUIScaleChanged final : geode::Event<EditorUIScaleChanged, bool(float pScale)> {
        using Event::Event;
    };

    /// get current editor ui scale
    /// @returns ui scale or 1.0f if editor isnt loaded
    UI_SCALING_DLL float getEditorUIScale();
    /// update editor ui scale regardless if editor ui scaling is enabled
    /// @param pScale ui scale
    /// @param pVanillaPositioning "by default this mod also fixes some inconsistencies in editor ui, this setting will revert that to be as consistent to the original as possible"
    UI_SCALING_DLL void setEditorUIScale(float pScale, bool pVanillaPositioning = false);
};