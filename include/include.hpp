#pragma once

#include <Geode/loader/Dispatch.hpp>

namespace nwo5::uiscaling {
    #undef MY_MOD_ID
    #define MY_MOD_ID "nwo5.ui-scaling"

    namespace impl {
        inline geode::Result<float> getEditorUIScale()
        GEODE_EVENT_EXPORT(&getEditorUIScale, ());
    }

    namespace EditorUI {
        /// event sent every time editor ui scale changed, and on editor init even if ui scaling is disabled
        /// @note runs after ui is scaled and positioned
        struct Changed final : geode::Event<Changed, bool(float pScale)> {
            using Event::Event;
        };

        /// get current editor ui scale
        /// @returns ui scale or 1.0f if editor isnt loaded or ui scaling not enabled
        inline float getScale() {
            return impl::getEditorUIScale().unwrapOr(1.0f);
        }

        /// set editor ui scale (regardless if editor ui scaling is enabled in mod settings)
        /// @param pScale ui scale
        /// @param pVanillaPositioning use this mods fixed positioning option instead of copying vanilla as closely as possible
        inline void setScale(float pScale, bool pVanillaPositioning = true, bool pScaleToolbar = true, bool pUseSafeArea = false, std::optional<float> pCustomSafeArea = std::nullopt)
        GEODE_EVENT_EXPORT_NORES(&setScale, (pScale, pVanillaPositioning, pScaleToolbar, pUseSafeArea, pCustomSafeArea));
    }

    namespace prelude {
        namespace uiscaling = nwo5::uiscaling;
    }

    #undef MY_MOD_ID
};