// Autogenerated icons
#include <tasks_icons.h>
#include <string>
#include "UFZ/UI.hpp"

namespace Scenes
{
    enum Scenes
    {
        MAIN_MENU,
        EDIT_MENU,
        NAME_TEXT_EDIT,
        DESCRIPTION_TEXT_EDIT,
        DESCRIPTION,
        DELETE,
        POPUP,
        SCENES_COUNT
    };
}

namespace Dialogs
{
    enum Dialogs
    {
        DESCRIPTION,
        DONE,
        RENAME,
        EDIT_DESCRIPTION,
        DELETE
    };
}

typedef std::vector<std::pair<std::string, std::string>> NoteContainer;

struct PopupTemplate
{
    std::string header;
    std::string text = "No description has been set";
    std::string dialogTextStorage;

    std::string inputTextBuffer;

    NoteContainer todo = {
        { "+ New task",                 "Describes a new task"                                                          },
        { "Write linux desktop guide",  "Write a detailed essay that explains how the Linux desktop works"              },
        { "Write flipper guide",        "Write a guide for the new Flipper Zero framework"                              },
        { "Publish flipper framework",  "Create a GitHub repository and CI workflows for the new UntitledFZ framework"  }
    };
    NoteContainer done{};

    NoteContainer* currentContainer = &todo;
    size_t currentNoteIndex = 0;
};

#define CTX(x) ((PopupTemplate*)(x))

namespace List
{
    void callback(void* context, uint32_t index)
    {
        auto* app = (UFZ::Application*)context;
        UNUSED(app->getSceneManager().handleCustomEvent(index));
    }

    template<Scenes::Scenes T>
    void enter(void* context)
    {
        auto* menu = GET_WIDGET_P(context, UFZ::Submenu, T);
        auto* ctx = CTX(menu->application->getUserPointer());

        menu->reset();

        if constexpr (T == Scenes::EDIT_MENU)
        {
            UNUSED(menu->setHeader(ctx->header.c_str())
                            .addItem("Description",         Dialogs::DESCRIPTION,        callback, menu->application)
                            .addItem((ctx->currentContainer == &ctx->todo ? "Mark as done" : "Mark as not done"), Dialogs::DONE, callback, menu->application)
                            .addItem("Rename",              Dialogs::RENAME,             callback, menu->application)
                            .addItem("Edit description",    Dialogs::EDIT_DESCRIPTION,   callback, menu->application)
                            .addItem("Delete",              Dialogs::DELETE,             callback, menu->application));
        }
        else
        {
            UNUSED(menu->setHeader(ctx->currentContainer == &ctx->todo ? "Tasks - TODO" : "Tasks - Done"));
            for (size_t i = 0; i < ctx->currentContainer->size(); i++)
                UNUSED(menu->addItem((*ctx->currentContainer)[i].first.c_str(), Scenes::POPUP + i, callback, menu->application));
        }
        GOTO_SCENE(menu->application, T);
    }

    template<Scenes::Scenes T>
    bool event(void* context, SceneManagerEvent event)
    {
        auto* app = (UFZ::Application*)context;
        bool consumed = false;
        if (event.type == SceneManagerEventTypeCustom)
        {
            auto* ctx = CTX(app->getUserPointer());

            if constexpr (T == Scenes::MAIN_MENU)
            {
                ctx->currentNoteIndex = (event.event - Scenes::POPUP);
                auto& a = (*ctx->currentContainer)[ctx->currentNoteIndex];

                ctx->header = "Editing: " + a.first + "!";
                ctx->text = a.second;

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
                        if (ctx->currentContainer == &ctx->todo)
                        {
                            if (ctx->todo.size() == 1)
                            {
                                scene = Scenes::EDIT_MENU;
                                break;
                            }
                            otherContainer = &ctx->done;
                        }
                        else
                            otherContainer = &ctx->todo;

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
    void exit(void* context)
    {
        auto* widget = GET_WIDGET_P(context, UFZ::Submenu, T);
        widget->reset();
    }
}

namespace Description
{
    void enter(void* context)
    {
        auto* popup = GET_WIDGET_P(context, UFZ::Popup, Scenes::DESCRIPTION);
        popup->reset();
        popup->setContext(popup->application)
                .setHeader("Description:", 64, 4, AlignCenter, AlignTop)
                .setIcon(-1, -1, nullptr)
                .setText(CTX(popup->application->getUserPointer())->text.c_str(), 4, 16, AlignLeft, AlignTop);
        GOTO_SCENE(popup->application, Scenes::DESCRIPTION);
    }

    bool event(void* context, SceneManagerEvent event)
    {
        UNUSED(context); UNUSED(event);
        return false;
    }

    void exit(void* context)
    {
        GET_WIDGET_P(context, UFZ::Popup, Scenes::DESCRIPTION)->reset();
    }
}

namespace DeleteDialog
{
    void callback(DialogExResult result, void* context)
    {
        auto* app = (UFZ::Application*)context;
        if (result == DialogExResultRight)
        {
            auto* ctx = CTX(app->getUserPointer());
            ctx->currentContainer->erase(ctx->currentContainer->begin() + static_cast<NoteContainer::difference_type>(ctx->currentNoteIndex));
        }
        app->getSceneManager().searchAndSwitchToAnotherScene(Scenes::MAIN_MENU); // Disallows returning to the popup using the back button
    }

    void enter(void* context)
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

    bool event(void* context, SceneManagerEvent event)
    {
        UNUSED(context); UNUSED(event);
        return false;
    }

    void exit(void* context)
    {
        GET_WIDGET_P(context, UFZ::DialogEx, Scenes::DELETE)->reset();
    }
}

namespace Input
{

    template<Scenes::Scenes T>
    void callback(void* context)
    {
        auto* app = (UFZ::Application*)context;
        auto* ctx = CTX(app->getUserPointer());
        ctx->inputTextBuffer.shrink_to_fit();
        if constexpr (T == Scenes::DESCRIPTION_TEXT_EDIT)
            (*ctx->currentContainer)[ctx->currentNoteIndex].second = ctx->inputTextBuffer;
        else
            (*ctx->currentContainer)[ctx->currentNoteIndex].first = ctx->inputTextBuffer;

        app->getSceneManager().searchAndSwitchToAnotherScene(Scenes::EDIT_MENU); // Disallows returning to the popup using the back button
    }

    template<Scenes::Scenes T>
    void enter(void* context)
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

        GOTO_SCENE(popup->application, T);
    }

    bool event(void* context, SceneManagerEvent event)
    {
        UNUSED(context); UNUSED(event);
        return false;
    }

    template<Scenes::Scenes T>
    void exit(void* context)
    {
        GET_WIDGET_P(context, UFZ::TextInput, T)->reset();
    }
}

#ifdef __cplusplus
extern "C"
{
#endif
int32_t tasks_app(void* p)
{
    UNUSED(p); UNUSED(tasks_app);
    UFZ::Submenu list{ List::enter<Scenes::MAIN_MENU>, List::event<Scenes::MAIN_MENU>, List::exit<Scenes::MAIN_MENU> };
    UFZ::Submenu editList{ List::enter<Scenes::EDIT_MENU>, List::event<Scenes::EDIT_MENU>, List::exit<Scenes::EDIT_MENU> };

    UFZ::TextInput inputName{ Input::enter<Scenes::NAME_TEXT_EDIT>, Input::event, Input::exit<Scenes::NAME_TEXT_EDIT> };
    UFZ::TextInput inputDescription{ Input::enter<Scenes::DESCRIPTION_TEXT_EDIT>, Input::event, Input::exit<Scenes::DESCRIPTION_TEXT_EDIT> };

    UFZ::Popup description{ Description::enter, Description::event, Description::exit };
    UFZ::DialogEx deleteDialog{ DeleteDialog::enter, DeleteDialog::event, DeleteDialog::exit };

    PopupTemplate tmpl;

    UFZ::Application application({ &list, &editList, &inputName, &inputDescription, &description, &deleteDialog }, &tmpl);
    return 0;
}
#ifdef __cplusplus
};
#endif