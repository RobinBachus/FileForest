#include <GLFW/glfw3.h>
#include <iostream>
#include "extern/imgui/imgui.h"
#include "extern/imgui/backends/imgui_impl_glfw.h"
#include "extern/imgui/backends/imgui_impl_opengl3.h"

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize glfw3" << std::endl;
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow *window = glfwCreateWindow(800, 600, "File Forest", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        std::cerr << "Failed to create window" << std::endl;
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    auto context = ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

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

        // Your rendering/UI code goes here
        static float f = 0.0f;
        static int counter = 0;

        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ResizeGrip, 0);
        ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, 0);

        ImGui::Begin("Hello, world!");

        // Style
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_None;
        if (ImGui::GetMouseCursor() == ImGuiMouseCursor_ResizeNWSE)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

        float defaultScale = ImGui::GetFont()->Scale;
        ImGui::GetFont()->Scale *= .7;
        ImGui::PushFont(ImGui::GetFont());

        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::GetFont()->Scale = defaultScale;
        ImGui::PopFont();

        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

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
