#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include "ShapeMeshes.h"

struct GLFWwindow;
class ShaderManager;

class SceneManager
{
public:
    SceneManager();
    ~SceneManager();

    void Initialize(int w, int h);
    void Render(GLFWwindow* window);

    void OnResize(int w, int h);
    void OnMouseMove(double xpos, double ypos);
    void OnScroll(double yoffset);
    void ProcessInput(GLFWwindow* window, float dt);

private:
    void PrepareScene();
    void RenderScene();

private:
    bool m_usePerspective = true;

    // Camera
    glm::vec3 m_camPos = glm::vec3(0.0f, 1.05f, 3.55f);
    glm::vec3 m_camFront = glm::normalize(glm::vec3(0.0f, -0.12f, -1.0f));
    glm::vec3 m_camUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float m_yaw = -90.0f;
    float m_pitch = -12.0f;
    float m_fov = 40.0f;

    float m_moveSpeed = 2.8f;

    int m_width = 800;
    int m_height = 600;

    // Mouse capture toggle
    bool   m_mouseCaptured = false;
    bool   m_firstMouse = true;
    double m_lastX = 0.0;
    double m_lastY = 0.0;

    ShaderManager* m_shader = nullptr;
    ShapeMeshes m_shapeMeshes;
};