#include "Scenes/Scenes.hpp"
#include "Scenes/List.hpp"
#include "Scenes/Input.hpp"
#include "Scenes/DeleteDialog.hpp"
#include "Scenes/Description.hpp"

#ifdef __cplusplus
extern "C"
{
#endif
int32_t tasks_app(void* p)
{
    UNUSED(p); UNUSED(tasks_app);
    UFZ::Application application{};

    UFZ::View view{};
    view.setDeferredSetupCallback([&application](UFZ::View& v) -> void {
        UNUSED(v.setContext(&application)
                .setInputCallback([](InputEvent* event, void* context) -> bool {
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
                }));
    });

    UFZ::Submenu list{ FTasks::List::enter<FTasks::Scenes::MAIN_MENU>, FTasks::List::event<FTasks::Scenes::MAIN_MENU>, FTasks::List::exit<FTasks::Scenes::MAIN_MENU>, { &view } };
    UFZ::Submenu editList{ FTasks::List::enter<FTasks::Scenes::EDIT_MENU>, FTasks::List::event<FTasks::Scenes::EDIT_MENU>, FTasks::List::exit<FTasks::Scenes::EDIT_MENU> };

    UFZ::TextInput inputName{ FTasks::Input::enter<FTasks::Scenes::NAME_TEXT_EDIT>, FTasks::Input::event, FTasks::Input::exit<FTasks::Scenes::NAME_TEXT_EDIT> };
    UFZ::TextInput inputDescription{ FTasks::Input::enter<FTasks::Scenes::DESCRIPTION_TEXT_EDIT>, FTasks::Input::event, FTasks::Input::exit<FTasks::Scenes::DESCRIPTION_TEXT_EDIT> };

    UFZ::Popup description{ FTasks::Description::enter, FTasks::Description::event, FTasks::Description::exit };
    UFZ::DialogEx deleteDialog{ FTasks::DeleteDialog::enter, FTasks::DeleteDialog::event, FTasks::DeleteDialog::exit };

    FTasks::ApplicationData data{};

    application.init({ &list, &editList, &inputName, &inputDescription, &description, &deleteDialog }, &data);
    FTasks::Data::save(application);
    return 0;
}
#ifdef __cplusplus
};
#endif