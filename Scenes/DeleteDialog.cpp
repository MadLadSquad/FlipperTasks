#include "DeleteDialog.hpp"

void FTasks::DeleteDialog::callback(DialogExResult result, void* context) noexcept
{
    auto* app = (UFZ::Application*)context;
    if (result == DialogExResultRight)
    {
        auto* ctx = CTX(app->getUserPointer());
        ctx->currentContainer->erase(ctx->currentContainer->begin() + static_cast<NoteContainer::difference_type>(ctx->currentNoteIndex));
    }
    UNUSED(app->getSceneManager().searchAndSwitchToAnotherScene(Scenes::MAIN_MENU)); // Disallows returning to the popup using the back button
}

void FTasks::DeleteDialog::enter(void* context) noexcept
{
    auto* popup = GET_WIDGET_P(context, UFZ::DialogEx, Scenes::DELETE);
    auto* ctx = CTX(popup->application->getUserPointer());

    ctx->dialogTextStorage = "Do you want to delete the following note: \"" + (*ctx->currentContainer)[ctx->currentNoteIndex].first + "\"?";

    popup->reset();
    popup->setContext(popup->application)
            .setHeader("Delete note?", 64, 4, AlignCenter, AlignTop)
            .setIcon(-1, -1, nullptr)
            .setText(ctx->dialogTextStorage.c_str(), 4, 16, AlignLeft, AlignTop)
            .setLeftButtonText("No")
            .setCenterButtonText(nullptr)
            .setRightButtonText("Yes")
            .setResultCallback(callback);
    GOTO_SCENE(popup->application, Scenes::DELETE);
}

bool FTasks::DeleteDialog::event(void* context, SceneManagerEvent event) noexcept
{
    UNUSED(context); UNUSED(event);
    return false;
}

void FTasks::DeleteDialog::exit(void* context)
{
    GET_WIDGET_P(context, UFZ::DialogEx, Scenes::DELETE)->reset();
}