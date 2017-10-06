#include <gl/Buffer.hpp>

using namespace NatVis;
using namespace std;

template <>
GLenum
NatVis::hidden::elementType<GLubyte>()
{
    return GL_UNSIGNED_BYTE;
};

template <>
GLenum
NatVis::hidden::elementType<GLushort>()
{
    return GL_UNSIGNED_SHORT;
};

template<>
GLenum
NatVis::hidden::elementType<GLuint>()
{
    return GL_UNSIGNED_INT;
};

