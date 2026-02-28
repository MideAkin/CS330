// MainCode.cpp (FULL REPLACEMENT)

#include <iostream>
#include <stdexcept>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "SceneManager.h"

static SceneManager* g_scene = nullptr;

static void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    if (g_scene) g_scene->OnResize(width, height);
}

static void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (g_scene) g_scene->OnMouseMove(xpos, ypos);
}

static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)xoffset;
    if (g_scene) g_scene->OnScroll(yoffset);
}

int main()
{
    try
    {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW.");

        // OpenGL 3.3 core
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        const int startW = 1280;
        const int startH = 720;

        GLFWwindow* window = glfwCreateWindow(startW, startH, "CS 330 Final - Desk Scene", nullptr, nullptr);
        if (!window)
            throw std::runtime_error("Failed to create GLFW window.");

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        // GLEW
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK)
            throw std::runtime_error("Failed to initialize GLEW.");

        glEnable(GL_DEPTH_TEST);

        SceneManager scene;
        g_scene = &scene;

        scene.Initialize(startW, startH);

        glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
        glfwSetCursorPosCallback(window, MouseCallback);
        glfwSetScrollCallback(window, ScrollCallback);

        // KEEP cursor visible (do NOT disable it)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        double lastTime = glfwGetTime();

        while (!glfwWindowShouldClose(window))
        {
            double now = glfwGetTime();
            float dt = static_cast<float>(now - lastTime);
            lastTime = now;

            glfwPollEvents();

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, GLFW_TRUE);

            scene.ProcessInput(window, dt);
            scene.Render(window);

            glfwSwapBuffers(window);
        }

        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
        return 1;
    }
}