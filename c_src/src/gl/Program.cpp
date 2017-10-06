#include <gl/Program.hpp>
#include <gl/GLException.hpp>

#include <algorithm>
#include <iostream>

using namespace std;
using namespace NatVis;

namespace
{
    constexpr int LOG_LENGTH = 1024;
}

void
NatVis::uniforms::uniformValue(GLint location, float f)
{
    glUniform1f(location, f);
}

void
NatVis::uniforms::uniformValue(GLint location, const array<float, 1>& vec)
{
    glUniform1fv(location, 1, vec.data());
}

void
NatVis::uniforms::uniformValue(GLint location, const array<float, 2>& vec)
{
    glUniform2fv(location, 1, vec.data());
}

void
NatVis::uniforms::uniformValue(GLint location, const array<float, 3>& vec)
{
    glUniform3fv(location, 1, vec.data());
}

void
NatVis::uniforms::uniformValue(GLint location, const array<float, 4>& vec)
{
    glUniform4fv(location, 1, vec.data());
}

Program::Program()
    : shouldDelete{true}
    , handle{glCreateProgram()}
{ }

Program::Program(Program&& from)
    : shouldDelete{from.shouldDelete}
    , handle{from.handle}
{
    // Don't let 'from' delete my program now that I own it!
    from.shouldDelete = false;
}

Program::~Program()
{
    if (shouldDelete)
    {
        glDeleteProgram(handle);
        shouldDelete = false;
    }
}

GLuint
Program::raw()
{
    return handle;
}

void
Program::use()
{
    glUseProgram(handle);
}

GLint
Program::uniformLocation(const string& uniform)
{
    return glGetUniformLocation(handle, uniform.c_str());
}

ProgramLinker&
ProgramLinker::vertexAttributes(const vector<string>& attribs)
{
    _vertexAttributes = attribs;
    return *this;
}

ProgramLinker&
ProgramLinker::attach(Shader& shader)
{
    _shaders.push_back(&shader);
    return *this;
}

Program
ProgramLinker::link()
{
    auto program = Program{};

    for (auto shader : _shaders)
    {
        glAttachShader(program.raw(), shader->raw());
    }

    const int size = _vertexAttributes.size();
    for (int idx = 0; idx < size; idx++)
    {
        auto attrib = _vertexAttributes[idx].c_str();
        glBindAttribLocation(program.raw(), idx, attrib);
    }

    glLinkProgram(program.raw());

    GLint linkSuccessful;
    glGetProgramiv(program.raw(), GL_LINK_STATUS, &linkSuccessful);
    if (linkSuccessful == GL_FALSE)
    {
        char infoLog[LOG_LENGTH];
        glGetProgramInfoLog(program.raw(), LOG_LENGTH, NULL, infoLog);
        throw GLException{ "Failed to link OpenGL shader program because"
                         , infoLog
                         };
    }

    return program;
}

