#pragma once
#include "Scenes.hpp"

namespace FTasks::List
{
    void callback(void* context, uint32_t index) noexcept;
    void viewInputEvent(UFZ::Application& application, UFZ::View& view) noexcept;
    bool viewInputEventCallback(InputEvent* event, void* context) noexcept;

    template<Scenes::Scenes T>
    void enter(void* context) noexcept
    {
        auto* menu = GET_WIDGET_P(context, UFZ::Submenu, T);
        auto* ctx = CTX(menu->application->getUserPointer());

        static bool bFirst = true;
        if (bFirst && T == Scenes::MAIN_MENU)
        {
            Data::load(*menu->application);
            bFirst = false;
        }

        menu->reset();

        if constexpr (T == Scenes::EDIT_MENU)
        {
            UNUSED(menu->setHeader(("Editing: " + (*ctx->currentContainer)[ctx->currentNoteIndex].first).c_str())
                           .addItem("Description",         Dialogs::DESCRIPTION,        callback, menu->application)
                           .addItem((ctx->currentContainer == &ctx->containers.todo ? "Mark as done" : "Mark as not done"), Dialogs::DONE, callback, menu->application)
                           .addItem("Rename",              Dialogs::RENAME,             callback, menu->application)
                           .addItem("Edit description",    Dialogs::EDIT_DESCRIPTION,   callback, menu->application)
                           .addItem("Delete",              Dialogs::DELETE,             callback, menu->application));
        }
        else
        {
            UNUSED(menu->setHeader(ctx->currentContainer == &ctx->containers.todo ? "Tasks - TODO" : "Tasks - Done"));
            for (size_t i = 0; i < ctx->currentContainer->size(); i++)
                UNUSED(menu->addItem((*ctx->currentContainer)[i].first.c_str(), Scenes::POPUP + i, callback, menu->application));
        }
        RENDER_VIEW(menu->application, T);
    }

    template<Scenes::Scenes T>
    bool event(void* context, SceneManagerEvent event) noexcept
    {
        auto* app = (UFZ::Application*)context;
        bool consumed = false;
        if (event.type == SceneManagerEventTypeCustom)
        {
            auto* ctx = CTX(app->getUserPointer());

            if constexpr (T == Scenes::MAIN_MENU)
            {
                ctx->currentNoteIndex = (event.event - Scenes::POPUP);

                if (ctx->currentContainer == &ctx->containers.todo && ctx->currentNoteIndex == 0) // We're making a new note
                {
                    ctx->containers.todo.emplace_back("Task name", "Description");
                    ctx->bMakingNewNote = true;
                    ctx->currentNoteIndex = ctx->containers.todo.size() - 1;

                    app->getSceneManager().nextScene(Scenes::NAME_TEXT_EDIT);
                }
                else
                    app->getSceneManager().nextScene(Scenes::EDIT_MENU);
            }
            else
            {
                Scenes::Scenes scene;
                NoteContainer* otherContainer;

                switch (event.event)
                {
                    case Dialogs::DESCRIPTION:
                        scene = Scenes::DESCRIPTION;
                        break;
                    case Dialogs::DONE:
                        if (ctx->currentContainer == &ctx->containers.todo)
                        {
                            if (ctx->containers.todo.size() == 1)
                            {
                                scene = Scenes::EDIT_MENU;
                                break;
                            }
                            otherContainer = &ctx->containers.done;
                        }
                        else
                            otherContainer = &ctx->containers.todo;

                        otherContainer->push_back((*ctx->currentContainer)[ctx->currentNoteIndex]);
                        ctx->currentContainer->erase(ctx->currentContainer->begin() + static_cast<NoteContainer::difference_type>(ctx->currentNoteIndex));

                        // Go back because the current view holds invalid references. Additionally, changing the scene
                        // would require an additional press of the back button to actually exit
                        UNUSED(app->getSceneManager().previousScene());
                        scene = Scenes::EDIT_MENU;
                        break;
                    case Dialogs::RENAME:
                        scene = Scenes::NAME_TEXT_EDIT;
                        break;
                    case Dialogs::EDIT_DESCRIPTION:
                        scene = Scenes::DESCRIPTION_TEXT_EDIT;
                        break;
                    case Dialogs::DELETE:
                        scene = Scenes::DELETE;
                        break;
                    default:
                        scene = Scenes::MAIN_MENU;
                        break;
                }
                // In case the scene we're going to is the same as the current scene don't do any navigation as this will
                // add the current scene to the last and current scenes, requiring an additional press of the back button
                // to go back
                if (scene != T)
                    app->getSceneManager().nextScene(scene);
            }
            consumed = true;
        }
        return consumed;
    }

    template<Scenes::Scenes T>
    void exit(void* context) noexcept
    {
        auto* widget = GET_WIDGET_P(context, UFZ::Submenu, T);
        widget->reset();
    }
}