# Editor UI Scaling & API
haiiii

## listening to scaling
just listen to the event, like this or smth
```cpp
#include <Geode/modify/EditorUI.hpp>
#include <nwo5.ui-scaling/include/include.hpp>

using namespace nwo5::uiscaling::prelude;

class $modify(EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) {
            return false;
        }

        this->addEventListener(uiscaling::EditorUI::Changed(), [] (float scale) {
            log::info("rawr");
        });

        return true;
    }
};
```
this activates once on editor init and whenever this mod changes editor ui scale

now, the actual main appeal of using this mod is as an optional api that also listens for other mods ui scaling
```cpp
#include <Geode/modify/EditorUI.hpp>
#include <nwo5.ui-scaling/include/compat.hpp>

using namespace nwo5::uiscaling::prelude;

class $modify(EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) {
            return false;
        }

        this->addEventListener(uiscaling::compat::EditorUI::Changed(), [] (float scale, auto) {
            log::info("rawr");
        });

        return true;
    }
};
```
thru some *shenanigans*, the event will always trigger on editor init (technically onenter, orrrr right before scene switch if neither this mod or tinker are installed or active) and also whenever this mod or tinkers ui scale changes

## scaling ui
check the header `editor-ui.hpp` ig