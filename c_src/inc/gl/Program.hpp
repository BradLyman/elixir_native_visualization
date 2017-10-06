#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <gl/Shader.hpp>

#include <GL/glew.h>

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
class ProgramLinker
{
public:
    ProgramLinker() = default;

    /**
     * Set the vertex attribute names for this shader program.
     * These should match the 'in' values from the vertex shader, and if they
     * are incorrect will cause link to throw.
     * This will effectively call glBindAttribLocation for each attribute with the
     * corresponding index.
     */
    ProgramLinker& vertexAttributes(const std::vector<std::string>& attribs);

    /**
     * Tell the builder to attach the shader to the program before linking.
     * References to the shaders will remain alive until link() is called.
     */
    ProgramLinker& attach(Shader& shader);

    /**
     * Link the shaders and vertex attributes to create the Program.
     * @throws GLException if there is a link error.
     */
    Program link();

private:
    std::vector<std::string> _vertexAttributes;
    std::vector<Shader*> _shaders;
};

} /* namespace NatVis */
#endif

