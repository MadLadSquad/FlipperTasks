#include "Input.hpp"

bool FTasks::Input::event(void* context, SceneManagerEvent event) noexcept
{
    if (event.type == SceneManagerEventTypeCustom)
    {
        FORCE_NEXT_SCENE((UFZ::Application*)context, event.event);
        return true;
    }
    return false;
}