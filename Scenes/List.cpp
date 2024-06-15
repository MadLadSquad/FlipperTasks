#include "List.hpp"

void FTasks::List::callback(void* context, uint32_t index) noexcept
{
    auto* app = (UFZ::Application*)context;
    UNUSED(app->getSceneManager().handleCustomEvent(index));
}