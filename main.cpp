#include "main.h"

#include <GLFW/glfw3.h>

#include "extern/imgui/imgui.h"
#include "extern/imgui/backends/imgui_impl_glfw.h"
#include "extern/imgui/backends/imgui_impl_opengl3.h"

#include <iostream>
#include <vector>
#include <filesystem>
#include <format>
#include <map>
#include <cmath>
#include <cstdint>
#include <chrono>
#include <handleapi.h>
#include <fileapi.h>

using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

// https://en.cppreference.com/w/cpp/filesystem/directory_entry/file_size
struct HumanReadable
{
    std::uintmax_t size{};

    string to_string()
    {
        int i{};
        double mantissa = size;
        for (; mantissa >= 1024.0; mantissa /= 1024.0, ++i)
        {
        }
        return format("{}{}", std::ceil(mantissa * 10.0) / 10.0, i["BKMGTPE"]);
    }
};

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        cerr << "Failed to initialize glfw3" << endl;
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow *window = glfwCreateWindow(800, 600, "File Forest", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        cerr << "Failed to create window" << endl;
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    auto context = ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // TODO: Make dynamic
    string c_dir = "C:\\Users\\robin\\Desktop\\Projects\\CPP\\FileForest";
    auto dirs = get_files_and_dirs(c_dir, true);

    // Setup ImGui GLFW and OpenGL3 bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ResizeGrip, 0);
        ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, 0);

        ImGui::Begin(c_dir.c_str());

        // TODO Add this or remove it
        // char *buff;
        // c_dir.copy(buff, c_dir.length(), 0);
        // ImGui::InputText("Current dir: ", buff, strlen(buff) * 2);

        // Style
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_None;
        if (ImGui::GetMouseCursor() == ImGuiMouseCursor_ResizeNWSE)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("Current dir: %s", c_dir.c_str());

        if (ImGui::Button("Get folder sizes"))
            insert_sizes(dirs, c_dir);

        ImGui::NewLine();

        ImGui::BeginGroup();
        display_files(c_dir, dirs, true);
        ImGui::EndGroup();

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render your UI elements here

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Close GLFW and exit
    glfwTerminate();
    return 0;
}

void display_files(string &c_dir, map<char, map<string, uintmax_t>> &dirs, const bool get_size)
{
    for (auto &i : dirs['D'])
    {
        ImGui::BeginGroup();
        if (ImGui::Button(format(".{}", i.first).c_str()))
        {
            if (i.first != "\\..")
                c_dir += i.first;
            else
                c_dir = c_dir.erase(get_last_index(c_dir, '\\'));

            dirs = get_files_and_dirs(c_dir, get_size);
        }
        if (i.second != -1)
        {
            ImGui::SameLine();
            ImGui::Text("size: %s (%d)", HumanReadable{i.second}.to_string().c_str(), i.second);
        }
        ImGui::EndGroup();
    }

    ImGui::NewLine();

    for (auto &i : dirs['F'])
    {
        ImGui::BeginGroup();
        ImGui::Text(i.first.c_str());
        display_size(i.second);
        ImGui::EndGroup();
    }
}

void display_size(uintmax_t size)
{
    if (size == -1)
        return;

    ImGui::SameLine();
    ImGui::NewLine();
    ImGui::SameLine();
    ImGui::Text("size: %s (%d)", HumanReadable{size}.to_string().c_str(), size);
}

map<char, map<string, uintmax_t>> get_files_and_dirs(const string &path, const bool get_size)
{
    map<char, map<string, uintmax_t>> _dirs{{'D', {}}, {'F', {}}};

    _dirs['D'].insert({"\\..", -1});

    for (auto &i : fs::directory_iterator(path))
    {
        char type = i.is_directory() ? 'D' : 'F';
        string _path = shorten_path(i.path(), path);
        _dirs[type].insert({_path, -1});
    }

    return _dirs;
}

void insert_sizes(std::map<char, std::map<std::string, uintmax_t>> &dirs, const std::string &path)
{
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile((path + "\\*").c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        // Handle error
        return;
    }

    uintmax_t total_size = 0;

    do
    {
        std::string filePath = path + "\\" + findData.cFileName;

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0)
            {
                total_size += dirs['D'][filePath];
            }
        }
        else
        {
            // This is a file.
            HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

            if (hFile == INVALID_HANDLE_VALUE)
            {
                // Handle error
                continue;
            }

            LARGE_INTEGER fileSize;
            if (!GetFileSizeEx(hFile, &fileSize))
            {
                // Handle error
                CloseHandle(hFile);
                continue;
            }

            dirs['F'][filePath] = fileSize.QuadPart;
            total_size += fileSize.QuadPart;

            CloseHandle(hFile);
        }
    } while (FindNextFile(hFind, &findData) != 0);

    FindClose(hFind);

    // Store the total size of the directory.
    dirs['D'][path] = total_size;
}

string shorten_path(const filesystem::path _path, const string c_dir)
{
    return _path.string().replace(0, c_dir.length(), "");
}

int get_last_index(string str, char target)
{
    for (int i = str.length() - 1; i >= 0; i--)
        if (str[i] == target)
            return i;

    return -1;
}