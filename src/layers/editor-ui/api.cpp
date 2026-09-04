#include <nwo5.silly-api/include/include.hpp>
#define GEODE_DEFINE_EVENT_EXPORTS
#include <include.hpp>
#include "editor-ui.hpp"

using namespace geode::prelude;

Result<float> nwo5::uiscaling::impl::getEditorUIScale() {
    if (auto ui = editor::ui<UIScalingEditorUI>()) {
        return Ok(ui->m_fields->scale);
    }
    
    return Ok(1.0f);
}
void nwo5::uiscaling::EditorUI::setScale(float pScale, bool pVanillaPositioning, bool pScaleToolbar, bool pUseSafeArea, std::optional<float> pCustomSafeArea) {
    if (auto ui = editor::ui<UIScalingEditorUI>()) {
        ui->updateUIScale(pScale, pVanillaPositioning, pScaleToolbar, pUseSafeArea, pCustomSafeArea);
    }
}