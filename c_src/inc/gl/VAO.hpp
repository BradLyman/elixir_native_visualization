#ifndef VAO_HPP
#define VAO_HPP

#include <GL/glew.h>

#include <gl/Buffer.hpp>

#include <string>
#include <array>
#include <vector>
#include <iostream>

namespace NatVis
{;

template <class Vertex>
class AttribBinder;

/**
 * This class represents an OpenGL Vertex Array Object.
 */
class Vao
{
public:
    /**
     * Create an OpenGL Vertex Array Object.
     */
    Vao();

    /**
     * Destroy the VAO.
     */
    ~Vao();

    /**
     * Transfer ownership of the OpenGL Vertex Array Object.
     */
    Vao(Vao&& from);

    /**
     * It is not possible to copy an OpenGL Vertex Array Object.
     */
    Vao(const Vao&) = delete;

    /**
     * A non-owning reference to the raw OpenGL Vertex Array Object.
     */
    GLuint raw() const;

    /**
     * Bind the VAO.
     */
    void bind() const;

    template <class Vertex>
    AttribBinder<Vertex> bindLayoutFor() {
        bind();
        return AttribBinder<Vertex>{*this};
    }

private:
    bool shouldDelete;
    GLuint handle;
};

struct IndexOffset
{
    std::size_t offset;
    int length;
};

template <class Vertex>
class AttribBinder
{
public:
    AttribBinder(Vao& vao) : vao{vao} {}
    AttribBinder(const AttribBinder&) = default;

    template <std::array<float, 1>::size_type length>
    AttribBinder& withAttrib(std::array<float, length> Vertex::*attrib)
    {
        static Vertex* nullVtx = 0;

        auto offset = (std::size_t)&(nullVtx->*attrib);
        attribs.push_back({ offset, length });
        return *this;
    }

    void toBuffer(Buffer<Vertex>& buffer)
    {
        auto scopedBinding = buffer.bind(BindTarget::Array);
        for (int index = 0; index < attribs.size(); index++)
        {
            glEnableVertexAttribArray(index);

            auto attrib = attribs[index];
            glVertexAttribPointer(
                index,
                attrib.length,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (const GLvoid*)attrib.offset
            );
        }
        THROW_ON_GL_ERROR();
    }

private:
    Vao& vao;
    std::vector<IndexOffset> attribs;
};

} /* namespace NatVis */
#endif

