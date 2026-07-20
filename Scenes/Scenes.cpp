#include "Scenes.hpp"

void FTasks::Data::writeContainerString(const FTasks::NoteContainer& container, const UFZ::File& file) noexcept
{
    for (size_t i = 0; i < container.size(); i++)
    {
        const auto& a = container[i];
        file.write((void*)a.first.data(), a.first.size());
        file.write((void*)newLine, 1);
        file.write((void*)a.second.data(), a.second.size());

        if (i < (container.size() - 1))
            file.write((void*)newLine, 1);
    }
}

void FTasks::Data::readContainerString(FTasks::NoteContainer& container, const UFZ::File& file) noexcept
{
    std::string content;
    content.resize(file.size() + 1);
    file.read(content.data(), content.size());

    container.clear();

    size_t pos;
    size_t previous = 0;
    size_t i = 1;
    for (; (pos = content.find(newLine, previous)) != std::string::npos; i++)
    {
        auto str = content.substr(previous, pos - previous);
        previous = pos + 1;
        if ((i % 2) == 0)
            container.back().second = str;
        else
            container.emplace_back(str, "");
    }
    if ((i % 2) == 0)
        container.back().second = content.substr(previous, pos);
    else
        container.emplace_back(content.substr(previous, pos), "");
}

namespace
{
    // Both helpers scope their own file handle, so the UFZ::File destructor closes it on every path
    void saveContainer(const UFZ::Application& application, const char* path, const FTasks::NoteContainer& container) noexcept
    {
        UFZ::File file{};
        if (file.open(application.getFilesystem(), path, FSAM_WRITE, FSOM_CREATE_ALWAYS))
            FTasks::Data::writeContainerString(container, file);
    }

    // An absent or empty file leaves the container at its default contents
    void loadContainer(const UFZ::Application& application, const char* path, FTasks::NoteContainer& container) noexcept
    {
        UFZ::File file{};
        if (file.open(application.getFilesystem(), path, FSAM_READ, FSOM_OPEN_EXISTING) && file.size() > 0)
            FTasks::Data::readContainerString(container, file);
    }
}

void FTasks::Data::save(const UFZ::Application& application) noexcept
{
    const auto* ctx = CTX(application.getUserPointer());
    saveContainer(application, TASKS_TODO_DATA_FILE, ctx->containers.todo);
    saveContainer(application, TASKS_DONE_DATA_FILE, ctx->containers.done);
}

void FTasks::Data::load(const UFZ::Application& application) noexcept
{
    auto* ctx = CTX(application.getUserPointer());
    loadContainer(application, TASKS_TODO_DATA_FILE, ctx->containers.todo);
    loadContainer(application, TASKS_DONE_DATA_FILE, ctx->containers.done);
}