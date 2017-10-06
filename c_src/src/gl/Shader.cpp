#include <gl/Shader.hpp>
#include <gl/GLException.hpp>
#include <GL/glew.h>

using namespace NatVis;
using namespace std;

namespace
{
    constexpr unsigned LOG_LENGTH = 1024;
}

Shader::Shader(ShaderType type)
    : shouldDestroy{true}
    , handle{glCreateShader(type)}
{ }

Shader::Shader(Shader&& from)
{
    swap(from.handle, handle);
    swap(from.shouldDestroy, shouldDestroy);

    // don't let 'from' destroy our handle when it goes out of scope!
    from.shouldDestroy = false;
}

Shader::~Shader()
{
    if (shouldDestroy)
    {
        glDeleteShader(handle);
        shouldDestroy = false;
    }
}

void
Shader::compile(const string& source)
{
    GLchar* sourceStrPtr[1];
    sourceStrPtr[0] = (GLchar *)source.c_str();

    glShaderSource(handle, 1, (const GLchar**)sourceStrPtr, nullptr);
    glCompileShader(handle);

    GLint compileSuccess;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &compileSuccess);

    if (compileSuccess == GL_FALSE)
    {
        char infoLog[LOG_LENGTH];
        glGetShaderInfoLog(handle, LOG_LENGTH, NULL, infoLog);
        throw GLException({ "Error while compiling shader!"
                          , "\n"
                          , infoLog
                          , "\n"
                          , source
                          });
    }
}

GLuint
Shader::raw()
{
    return handle;
}

