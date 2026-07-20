#pragma once
#include "Scenes.hpp"

namespace FTasks::Input
{
    template<Scenes::Scenes T>
    void callback(void* context) noexcept
    {
        const auto* app = static_cast<UFZ::Application*>(context);
        auto* ctx = CTX(app->getUserPointer());
        // TextInput fills the 128-byte buffer allocated in enter() with a NUL-terminated string, leaving the
        // std::string at its full 128-byte size. Trim it back to the text the user actually typed: the padding
        // is invisible through c_str() but writeContainerString writes size() bytes, so it would end up on disk
        ctx->inputTextBuffer.resize(std::strlen(ctx->inputTextBuffer.c_str()));

        if constexpr (T == Scenes::DESCRIPTION_TEXT_EDIT)
            (*ctx->currentContainer)[ctx->currentNoteIndex].second = ctx->inputTextBuffer;
        else
            (*ctx->currentContainer)[ctx->currentNoteIndex].first = ctx->inputTextBuffer;

        // Handle navigation when editing and creating new notes
        auto id = Scenes::EDIT_MENU;
        if (ctx->bMakingNewNote)
        {
            if constexpr (T == Scenes::DESCRIPTION_TEXT_EDIT)
            {
                id = Scenes::MAIN_MENU;

                ctx->bMakingNewNote = false; // Note successfully created
            }
            else
                id = Scenes::DESCRIPTION_TEXT_EDIT;
        }
        SEND_CUSTOM_EVENT(app, id);
    }

    template<Scenes::Scenes T>
    void enter(void* context) noexcept
    {
        auto* popup = GET_WIDGET_P(context, UFZ::TextInput, T);
        auto* ctx = CTX(popup->application->getUserPointer());

        if constexpr (T == Scenes::DESCRIPTION_TEXT_EDIT)
        {
            popup->setHeaderText("New description");
            ctx->inputTextBuffer = (*ctx->currentContainer)[ctx->currentNoteIndex].second;
        }
        else
        {
            popup->setHeaderText("New name");
            ctx->inputTextBuffer = (*ctx->currentContainer)[ctx->currentNoteIndex].first;
        }
        ctx->inputTextBuffer.resize(128);
        popup->setResultCallback(callback<T>, popup->application, ctx->inputTextBuffer.data(), ctx->inputTextBuffer.size(), false);

        RENDER_VIEW(popup->application, T);
    }

    bool event(void* context, SceneManagerEvent event) noexcept;

    template<Scenes::Scenes T>
    void exit(void* context) noexcept
    {
        GET_WIDGET_P(context, UFZ::TextInput, T)->reset();
    }
}