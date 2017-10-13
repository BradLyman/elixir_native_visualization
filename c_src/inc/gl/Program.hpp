#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <GL/glew.h>

#include <gl/Shader.hpp>

#include <array>
#include <vector>
#include <string>

namespace NatVis
{
namespace uniforms
{
    /**
     * Set the value of a scalar float uniform.
     */
    void uniformValue(GLint location, float f);

    /**
     * Set the value of a 1-element float vector.
     */
    void uniformValue(GLint location, const std::array<float, 1>& vec);

    /**
     * Set the value of a 2-element float vector.
     */
    void uniformValue(GLint location, const std::array<float, 2>& vec);

    /**
     * Set the value of a 3-element float vector.
     */
    void uniformValue(GLint location, const std::array<float, 3>& vec);

    /**
     * Set the value of a 4-element float vector.
     */
    void uniformValue(GLint location, const std::array<float, 4>& vec);
}

class ConfigRenderPipeline;

/**
 * This class represents an OpenGL Program object.
 */
class Program
{
public:
    /**
     * Create an OpenGL shader program.
     */
    Program();

    /**
     * It is not possible to copy OpenGL shader programs.
     */
    Program(const Program&) = delete;

    /**
     * Transfer ownership of the OpenGL shader program from another Program.
     */
    Program(Program&& from);

    /**
     * Delete the shader program.
     */
    ~Program();

    /**
     * Attach shaders and specify vertex attributes for this program.
     */
    ConfigRenderPipeline configRenderPipeline();

    /**
     * Get a non-owning reference to the raw OpenGL program object.
     */
    GLuint raw();

    /**
     * Use this program for the next OpenGL draw.
     */
    void use();

    /**
     * Lookup a uniform location in the program.
     * Effectively just calls glGetUniformLocation.
     */
    GLint uniformLocation(const std::string& uniform);

    /**
     * Set a uniform in the program.
     * To enable for a type T define an overload of
     * uniformValue(GLint location, const T& t)
     */
    template <class UType>
    void uniform(GLint location, UType&& type)
    {
        NatVis::uniforms::uniformValue(location, std::forward<UType>(type));
    }

private:
    bool shouldDelete;
    GLuint handle;
};

/**
 * This class constructs an OpenGL Program by linking shaders and attributes.
 */
class ConfigRenderPipeline
{
public:
    ConfigRenderPipeline(Program& program);
    ConfigRenderPipeline(ConfigRenderPipeline&&) = default;
    ConfigRenderPipeline(const ConfigRenderPipeline&) = default;

    /**
     * Set the vertex attribute names for this shader program.
     * Calls are order sensative, the first call will set the first attribute,
     * the second sets the second attribute, and so on.
     * This will call glBindAttribLocation for each attribute with the
     * corresponding index.
     */
    ConfigRenderPipeline& withAttribute(const std::string atribName);

    /**
     * Attach a shader to this program instance for linking.
     */
    ConfigRenderPipeline& withShader(Shader& shader);

    /**
     * Link the shaders and vertex attributes to the program.
     * @throws GLException if there is a link error.
     */
    void link();

private:
    std::vector<std::string> vertexAttributes;
    std::vector<Shader*> shaders;
    Program& program;
};

} /* namespace NatVis */
#endif

