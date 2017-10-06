#include <gl/VAO.hpp>

using namespace std;
using namespace NatVis;

Vao::Vao()
    : shouldDelete{true}
{
    glCreateVertexArrays(1, &handle);
}

Vao::~Vao()
{
    if (shouldDelete)
    {
        glDeleteVertexArrays(1, &handle);
        shouldDelete = false;
    }
}

Vao::Vao(Vao&& from)
    : shouldDelete{from.shouldDelete}
    , handle{from.handle}
{
    from.shouldDelete = false;
}

GLuint
Vao::raw() const
{
    return handle;
}

void
Vao::bind() const
{
    glBindVertexArray(handle);
}

