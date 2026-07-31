#pragma once

#include <nwo5.silly-api/include/settings/include.hpp>

using namespace nwo5::settings::prelude;

namespace Settings {
    inline Setting<bool> settingsButtonInEditor{"settings-button-in-editor"};

    inline Setting<bool> editorUIEnabled{"editor-ui-enabled"};
    inline Setting<float> editorUIScaling{"editor-ui-scaling"};
    inline Setting<bool> editorUIVanillaPositioning{"editor-ui-vanilla-positioning"};
    inline Setting<bool> editorUIScaleToolbar{"editor-ui-scale-toolbar"};
    inline Setting<bool> editorUICenterSlider{"editor-ui-center-slider"};
}