#pragma once

// IMPORTANT: GLEW must be included BEFORE GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Forward declare to avoid circular includes
class SceneManager;

class ViewManager
{
public:
    ViewManager();
    ~ViewManager();

    bool Initialize(SceneManager* scene, int width, int height, const char* title);
    void Run();

    GLFWwindow* GetWindow() const { return m_window; }

private:
    GLFWwindow* m_window = nullptr;
    SceneManager* m_scene = nullptr;

    int m_width = 800;
    int m_height = 600;

    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};