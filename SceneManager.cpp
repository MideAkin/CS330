#include "SceneManager.h"
#include "ShaderManager.h"

// IMPORTANT: include order
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

// ------------------------------------------------------------
// SceneManager.cpp - FULL REPLACEMENT (Structured Desk Scene)
//
// Controls:
//   WASD  = move
//   Q/E   = down/up
//   Shift = speed boost
//   M     = toggle mouse capture (mouse look on/off)
//   Scroll= FOV zoom
//   R     = reset camera
// ------------------------------------------------------------

namespace
{
    float Clampf(float v, float lo, float hi) { return std::max(lo, std::min(hi, v)); }

    // Centered mesh helper
    glm::mat4 MakeModel(const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& rotDeg = glm::vec3(0.0f))
    {
        glm::mat4 m(1.0f);
        m = glm::translate(m, pos);
        if (rotDeg.x != 0.0f) m = glm::rotate(m, glm::radians(rotDeg.x), glm::vec3(1, 0, 0));
        if (rotDeg.y != 0.0f) m = glm::rotate(m, glm::radians(rotDeg.y), glm::vec3(0, 1, 0));
        if (rotDeg.z != 0.0f) m = glm::rotate(m, glm::radians(rotDeg.z), glm::vec3(0, 0, 1));
        m = glm::scale(m, scale);
        return m;
    }

    // Put a centered mesh on a base Y plane
    glm::mat4 OnBaseY(float x, float baseY, float z, const glm::vec3& scale, const glm::vec3& rotDeg = glm::vec3(0.0f))
    {
        return MakeModel(glm::vec3(x, baseY + scale.y * 0.5f, z), scale, rotDeg);
    }

    // A real hinged box: pivot at (hingeX, hingeY, hingeZ), rotate around X, then lift by half height
    glm::mat4 HingedBoxX(float hingeX, float hingeY, float hingeZ, const glm::vec3& scale, float tiltDegX, float yawDegY = 0.0f)
    {
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(hingeX, hingeY, hingeZ));
        if (yawDegY != 0.0f) m = glm::rotate(m, glm::radians(yawDegY), glm::vec3(0, 1, 0));
        m = glm::rotate(m, glm::radians(tiltDegX), glm::vec3(1, 0, 0));
        m = glm::translate(m, glm::vec3(0.0f, scale.y * 0.5f, 0.0f));
        m = glm::scale(m, scale);
        return m;
    }
}

SceneManager::SceneManager()
{
    m_shader = nullptr;

    // Safe starting camera
    m_camPos = glm::vec3(0.0f, 1.00f, 3.30f);
    m_camFront = glm::normalize(glm::vec3(0.0f, -0.12f, -1.0f));
    m_camUp = glm::vec3(0.0f, 1.0f, 0.0f);

    m_yaw = -90.0f;
    m_pitch = -12.0f;

    m_fov = 40.0f;
    m_moveSpeed = 2.6f;

    m_mouseCaptured = false;
    m_firstMouse = true;
    m_usePerspective = true;
}

SceneManager::~SceneManager()
{
    delete m_shader;
    m_shader = nullptr;
}

void SceneManager::Initialize(int w, int h)
{
    m_width = w;
    m_height = h;
    PrepareScene();
}

void SceneManager::PrepareScene()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.10f, 0.10f, 0.12f, 1.0f);

    m_shader = new ShaderManager();
    m_shader->Initialize();

    // Your existing textures
    m_shader->CreateGLTexture("textures/wood.jpg", "wood");
    m_shader->CreateGLTexture("textures/ceramic.jpg", "ceramic");

    m_shader->Use();
    m_shader->SetInt("uTexture0", 0);
    m_shader->SetInt("uTexture1", 1);
    m_shader->SetFloat("uBlend", 0.0f);
    m_shader->SetVec2("uUVScale", 1.0f, 1.0f);

    // Meshes
    m_shapeMeshes.LoadPlaneMesh();
    m_shapeMeshes.LoadBoxMesh();
    m_shapeMeshes.LoadCylinderMesh();
    m_shapeMeshes.LoadConeMesh();
}

void SceneManager::Render(GLFWwindow* window)
{
    (void)window;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::lookAt(m_camPos, m_camPos + m_camFront, m_camUp);
    float aspect = (m_height > 0) ? (float)m_width / (float)m_height : 1.0f;

    glm::mat4 proj;
    if (m_usePerspective)
        proj = glm::perspective(glm::radians(m_fov), aspect, 0.1f, 200.0f);
    else
    {
        float s = 4.5f;
        proj = glm::ortho(-s * aspect, s * aspect, -s, s, 0.1f, 200.0f);
    }

    m_shader->Use();
    m_shader->SetMat4("uView", &view[0][0]);
    m_shader->SetMat4("uProj", &proj[0][0]);
    m_shader->SetVec3("uViewPos", m_camPos.x, m_camPos.y, m_camPos.z);

    // Softer lighting (so wall isn't blown out)
    m_shader->SetVec3("uLightPos1", 1.7f, 1.35f, 0.2f);
    m_shader->SetVec3("uLightColor1", 0.80f, 0.74f, 0.64f);

    m_shader->SetVec3("uLightPos2", -2.6f, 2.8f, 2.0f);
    m_shader->SetVec3("uLightColor2", 0.12f, 0.14f, 0.18f);

    // Single-light fallback (some templates use these)
    m_shader->SetVec3("uLightPos", 1.7f, 1.35f, 0.2f);
    m_shader->SetVec3("uLightColor", 0.80f, 0.74f, 0.64f);

    RenderScene();
}

void SceneManager::RenderScene()
{
    glm::mat4 model;

    // Desk plane at y=0
    const float deskY = 0.0f;

    // =========================================================
    // DESK
    // =========================================================
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, deskY, 0.0f));
    model = glm::scale(model, glm::vec3(6.2f, 1.0f, 3.4f));
    m_shader->SetMat4("uModel", &model[0][0]);
    m_shader->SetVec2("uUVScale", 3.5f, 2.2f);
    m_shader->BindTexture("wood", 0);
    m_shapeMeshes.DrawPlaneMesh();

    // =========================================================
    // WALL
    // =========================================================
    {
        glm::vec3 wallScale(7.0f, 2.6f, 0.12f);
        model = OnBaseY(0.0f, 0.35f, -2.2f, wallScale);
        m_shader->SetMat4("uModel", &model[0][0]);
        m_shader->SetVec2("uUVScale", 0.20f, 0.20f);
        m_shader->BindTexture("ceramic", 0);
        m_shapeMeshes.DrawBoxMesh();
    }

    // =========================================================
    // LAPTOP (base + hinge screen)
    // =========================================================
    const float laptopX = -0.35f;
    const float laptopZ = -0.05f;

    glm::vec3 baseScale(1.90f, 0.10f, 1.25f);
    glm::vec3 kbScale(1.72f, 0.03f, 1.05f);
    glm::vec3 tpScale(0.55f, 0.02f, 0.35f);

    // Base
    model = OnBaseY(laptopX, deskY, laptopZ, baseScale);
    m_shader->SetMat4("uModel", &model[0][0]);
    m_shader->SetVec2("uUVScale", 1.0f, 1.0f);
    m_shader->BindTexture("ceramic", 0);
    m_shapeMeshes.DrawBoxMesh();

    // Keyboard inset
    model = OnBaseY(laptopX, deskY + 0.07f, laptopZ + 0.08f, kbScale);
    m_shader->SetMat4("uModel", &model[0][0]);
    m_shader->BindTexture("wood", 0);
    m_shapeMeshes.DrawBoxMesh();

    // Trackpad
    model = OnBaseY(laptopX, deskY + 0.075f, laptopZ + 0.55f, tpScale);
    m_shader->SetMat4("uModel", &model[0][0]);
    m_shader->BindTexture("ceramic", 0);
    m_shapeMeshes.DrawBoxMesh();

    // Screen (real hinge pivot)
    {
        glm::vec3 screenScale(1.90f, 1.18f, 0.06f);

        float baseTop = deskY + baseScale.y;
        float hingeX = laptopX;
        float hingeY = baseTop;
        float hingeZ = laptopZ - (baseScale.z * 0.5f) + 0.02f; // rear edge of base

        // Tilt back like a laptop (tweak between 28..38 if desired)
        float tilt = 32.0f;

        glm::mat4 screen = HingedBoxX(hingeX, hingeY, hingeZ, screenScale, tilt, 180.0f);

        m_shader->SetMat4("uModel", &screen[0][0]);
        m_shader->BindTexture("ceramic", 0);
        m_shapeMeshes.DrawBoxMesh();

        // Screen panel inset (same transform + tiny push forward)
        glm::mat4 panel = screen;
        panel = glm::translate(panel, glm::vec3(0.0f, 0.0f, 0.06f));
        m_shader->SetMat4("uModel", &panel[0][0]);
        m_shader->BindTexture("wood", 0);
        m_shapeMeshes.DrawBoxMesh();
    }

    // =========================================================
    // MUG
    // =========================================================
    const float mugX = -2.05f;
    const float mugZ = 0.15f;

    glm::vec3 mugScale(0.28f, 0.38f, 0.28f);
    glm::vec3 innerScale(0.23f, 0.28f, 0.23f);

    model = OnBaseY(mugX, deskY, mugZ, mugScale);
    m_shader->SetMat4("uModel", &model[0][0]);
    m_shader->BindTexture("ceramic", 0);
    m_shapeMeshes.DrawCylinderMesh();

    model = OnBaseY(mugX, deskY + 0.06f, mugZ, innerScale);
    m_shader->SetMat4("uModel", &model[0][0]);
    m_shader->BindTexture("wood", 0);
    m_shapeMeshes.DrawCylinderMesh();

    // Handle (attached)
    model = OnBaseY(mugX + 0.23f, deskY + 0.10f, mugZ, glm::vec3(0.10f, 0.18f, 0.10f));
    m_shader->SetMat4("uModel", &model[0][0]);
    m_shader->BindTexture("ceramic", 0);
    m_shapeMeshes.DrawBoxMesh();

    model = OnBaseY(mugX + 0.18f, deskY + 0.10f, mugZ, glm::vec3(0.06f, 0.12f, 0.06f));
    m_shader->SetMat4("uModel", &model[0][0]);
    m_shader->BindTexture("wood", 0);
    m_shapeMeshes.DrawBoxMesh();

    // =========================================================
    // NOTEBOOK + PEN
    // =========================================================
    const float noteX = 1.55f;
    const float noteZ = 0.90f;

    model = OnBaseY(noteX, deskY, noteZ, glm::vec3(1.35f, 0.08f, 0.95f), glm::vec3(0, 18.0f, 0));
    m_shader->SetMat4("uModel", &model[0][0]);
    m_shader->BindTexture("ceramic", 0);
    m_shapeMeshes.DrawBoxMesh();

    model = OnBaseY(noteX, deskY + 0.055f, noteZ, glm::vec3(1.28f, 0.04f, 0.88f), glm::vec3(0, 18.0f, 0));
    m_shader->SetMat4("uModel", &model[0][0]);
    m_shader->BindTexture("wood", 0);
    m_shapeMeshes.DrawBoxMesh();

    model = OnBaseY(1.42f, deskY + 0.10f, 0.78f, glm::vec3(0.035f, 0.035f, 1.05f), glm::vec3(0, 30.0f, 0));
    m_shader->SetMat4("uModel", &model[0][0]);
    m_shader->BindTexture("ceramic", 0);
    m_shapeMeshes.DrawCylinderMesh();

    model = OnBaseY(1.83f, deskY + 0.10f, 0.30f, glm::vec3(0.055f, 0.11f, 0.055f), glm::vec3(90.0f, 30.0f, 0));
    m_shader->SetMat4("uModel", &model[0][0]);
    m_shapeMeshes.DrawConeMesh();

    // =========================================================
    // LAMP (base + post + shade)
    // =========================================================
    const float lampX = 2.15f;
    const float lampZ = -0.35f;

    glm::vec3 lampBase(0.42f, 0.06f, 0.42f);
    glm::vec3 lampPost(0.06f, 1.05f, 0.06f);

    model = OnBaseY(lampX, deskY, lampZ, lampBase);
    m_shader->SetMat4("uModel", &model[0][0]);
    m_shader->BindTexture("ceramic", 0);
    m_shapeMeshes.DrawCylinderMesh();

    model = OnBaseY(lampX, deskY + lampBase.y, lampZ, lampPost);
    m_shader->SetMat4("uModel", &model[0][0]);
    m_shapeMeshes.DrawCylinderMesh();

    model = MakeModel(glm::vec3(1.20f, deskY + 1.40f, -0.95f),
        glm::vec3(0.32f, 0.45f, 0.32f),
        glm::vec3(110.0f, 0.0f, 0.0f));
    m_shader->SetMat4("uModel", &model[0][0]);
    m_shapeMeshes.DrawConeMesh();
}

void SceneManager::OnResize(int w, int h)
{
    if (h <= 0) h = 1;
    m_width = w;
    m_height = h;
    glViewport(0, 0, w, h);
}

void SceneManager::OnMouseMove(double xpos, double ypos)
{
    if (!m_mouseCaptured)
    {
        m_firstMouse = true;
        return;
    }

    if (m_firstMouse)
    {
        m_lastX = xpos;
        m_lastY = ypos;
        m_firstMouse = false;
        return;
    }

    double xoffset = xpos - m_lastX;
    double yoffset = m_lastY - ypos;

    m_lastX = xpos;
    m_lastY = ypos;

    const float sensitivity = 0.08f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    m_yaw += (float)xoffset;
    m_pitch += (float)yoffset;
    m_pitch = Clampf(m_pitch, -89.0f, 89.0f);

    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_camFront = glm::normalize(front);
}

void SceneManager::OnScroll(double yoffset)
{
    m_fov -= (float)yoffset * 2.0f;
    m_fov = Clampf(m_fov, 25.0f, 65.0f);
}

void SceneManager::ProcessInput(GLFWwindow* window, float dt)
{
    if (!window) return;
    if (dt > 0.05f) dt = 0.05f;

    // Toggle mouse capture
    static bool mWasDown = false;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        if (!mWasDown)
        {
            m_mouseCaptured = !m_mouseCaptured;
            m_firstMouse = true;

            glfwSetInputMode(window, GLFW_CURSOR,
                m_mouseCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

            mWasDown = true;
        }
    }
    else
    {
        mWasDown = false;
    }

    float speed = m_moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
    {
        speed *= 2.25f;
    }

    float v = speed * dt;
    glm::vec3 right = glm::normalize(glm::cross(m_camFront, m_camUp));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) m_camPos += m_camFront * v;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) m_camPos -= m_camFront * v;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) m_camPos -= right * v;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) m_camPos += right * v;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) m_camPos -= m_camUp * v;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) m_camPos += m_camUp * v;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        m_camPos = glm::vec3(0.0f, 1.00f, 3.30f);
        m_camFront = glm::normalize(glm::vec3(0.0f, -0.12f, -1.0f));
        m_camUp = glm::vec3(0.0f, 1.0f, 0.0f);

        m_yaw = -90.0f;
        m_pitch = -12.0f;

        m_fov = 40.0f;
        m_moveSpeed = 2.6f;
        m_firstMouse = true;
    }
}