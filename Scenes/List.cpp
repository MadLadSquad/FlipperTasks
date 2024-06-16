#include "List.hpp"

void FTasks::List::callback(void* context, uint32_t index) noexcept
{
    auto* app = (UFZ::Application*)context;
    UNUSED(app->getSceneManager().handleCustomEvent(index));
}

void FTasks::List::viewInputEvent(UFZ::Application& application, UFZ::View& view) noexcept
{
    UNUSED(view.setContext(&application).setInputCallback(viewInputEventCallback));
}

bool FTasks::List::viewInputEventCallback(InputEvent* event, void* context) noexcept
{
    if (event == nullptr || context == nullptr)
        return false;
    if (event->type == InputTypePress)
    {
        if (event->key == InputKeyLeft || event->key == InputKeyRight)
        {
            auto* app = (UFZ::Application*)context;
            auto* ctx = CTX(app->getUserPointer());

            ctx->currentContainer = ctx->currentContainer == &ctx->containers.todo ? &ctx->containers.done : &ctx->containers.todo;
            ctx->currentNoteIndex = 0;

            UNUSED(app->getSceneManager().searchAndSwitchToAnotherScene(FTasks::Scenes::MAIN_MENU));
            return true;
        }
    }
    return false;
}