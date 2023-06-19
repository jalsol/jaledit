#include "app.h"

#include <assert.h>

#include "editor.h"

void app_render(App app) {
    switch (app.view_type) {
    case VIEW_EDIT:
        editor_render(app.view.editor);
        break;
    case VIEW_BROWSE:
    default:
        assert(0 && "unreachable");
    }
}