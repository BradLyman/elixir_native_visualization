#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>

#include <string>

namespace NatVis
{;

enum ShaderType
{
    VERTEX = GL_VERTEX_SHADER,
    FRAGMENT = GL_FRAGMENT_SHADER,
    GEOMETRY = GL_GEOMETRY_SHADER,
};

/**
 * This class is responsible for the ownership and management of an OpenGL
 * shader.
 */
class Shader
{
public:
    /**
     * Create a new shader.
     */
    Shader(ShaderType type);

    /**
     * Copying a compiled shader is not allowed.
     */
    Shader(const Shader&) = delete;

    /**
     * Move the shader.
     */
    Shader(Shader&&);

    /**
     * Destroy the shader.
     */
    ~Shader();

    /**
     * Loads the source code for this shader and compiles it.
     * @throws GLException if there is an issue while compiling the shader.
     */
    void compile(const std::string& source);

    /**
     * Return a non-owning reference to the OpenGL shader.
     */
    GLuint raw();
private:
    bool shouldDestroy;
    GLuint handle;
};

}; /* namespace NatVis */

#endif
