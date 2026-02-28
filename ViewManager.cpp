#include "ViewManager.h"
#include "SceneManager.h"

#include <iostream>

ViewManager::ViewManager() {}
ViewManager::~ViewManager()
{
    if (m_window)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

bool ViewManager::Initialize(SceneManager* scene, int width, int height, const char* title)
{
    m_scene = scene;
    m_width = width;
    m_height = height;

    if (!glfwInit())
    {
        std::cerr << "ERROR: glfwInit() failed\n";
        return false;
    }

    // Core profile OpenGL 3.3 (SNHU standard)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_width, m_height, title, nullptr, nullptr);
    if (!m_window)
    {
        std::cerr << "ERROR: glfwCreateWindow() failed\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // vsync (smoother input + timing)

    // IMPORTANT: init glew AFTER context exists
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "ERROR: glewInit() failed\n";
        return false;
    }

    glViewport(0, 0, m_width, m_height);

    // Store pointer for callbacks
    glfwSetWindowUserPointer(m_window, this);

    // Callbacks
    glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(m_window, MouseCallback);
    glfwSetScrollCallback(m_window, ScrollCallback);

    // Scene init
    if (m_scene)
        m_scene->Initialize(m_width, m_height);

    return true;
}

void ViewManager::Run()
{
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(m_window))
    {
        double now = glfwGetTime();
        float dt = static_cast<float>(now - lastTime);
        lastTime = now;

        if (m_scene)
            m_scene->ProcessInput(m_window, dt);

        if (m_scene)
            m_scene->Render(m_window);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void ViewManager::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    ViewManager* self = static_cast<ViewManager*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    self->m_width = width;
    self->m_height = height;

    if (self->m_scene)
        self->m_scene->OnResize(width, height);
}

void ViewManager::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    ViewManager* self = static_cast<ViewManager*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    if (self->m_scene)
        self->m_scene->OnMouseMove(xpos, ypos);
}

void ViewManager::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)xoffset;

    ViewManager* self = static_cast<ViewManager*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    if (self->m_scene)
        self->m_scene->OnScroll(yoffset);
}