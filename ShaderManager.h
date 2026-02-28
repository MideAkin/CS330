#pragma once

#include <string>
#include <map>

#include <GL/glew.h>

class ShaderManager
{
public:
    ShaderManager();
    ~ShaderManager();

    // Compile/link shader program
    void Initialize();

    // Texture functions
    void CreateGLTexture(const std::string& path, const std::string& tag);
    void BindTexture(const std::string& tag, int unit) const;

    // Uniform helpers
    void SetInt(const std::string& name, int v) const;
    void SetVec2(const std::string& name, float x, float y) const;
    void SetVec3(const std::string& name, float x, float y, float z) const;
    void SetMat4(const std::string& name, const float* m4) const;

private:
    GLuint CompileShader(GLenum type, const std::string& src);
    GLuint LinkProgram(GLuint vs, GLuint fs);

private:
    GLuint m_program = 0;
    std::map<std::string, GLuint> m_textures;
};
