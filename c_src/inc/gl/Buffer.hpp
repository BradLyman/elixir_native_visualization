#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <gl/GLException.hpp>

#include <GL/glew.h>

#include <vector>
#include <memory>
#include <iostream>

namespace NatVis
{;

enum Primitive
{
    Points = GL_POINTS,

    Lines = GL_LINES,
    LineLoop = GL_LINE_LOOP,
    LinesAdjacency = GL_LINES_ADJACENCY,
    LineStrip = GL_LINE_STRIP,
    LineStripAdjacency = GL_LINE_STRIP_ADJACENCY,

    Triangles = GL_TRIANGLES,
    TrianglesAdjacency = GL_TRIANGLES_ADJACENCY,
    TriangleStrip = GL_TRIANGLE_STRIP,
    TriangleStripAdjacency = GL_TRIANGLE_STRIP_ADJACENCY,
    TriangleFan = GL_TRIANGLE_FAN,

    Patches = GL_PATCHES,
};

enum BindTarget
{
    /** Vertex Attributes */
    Array = GL_ARRAY_BUFFER,
    /** Atomic counter Storage -- Min version 4.2  */
    AtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
    /** Buffer copy source */
    CopyRead = GL_COPY_READ_BUFFER,
    /** Buffer copy destination */
    CopyWrite = GL_COPY_WRITE_BUFFER,
    /** Indirect compute dispatch commands -- Min version 4.3 */
    DispatchIndirect = GL_DISPATCH_INDIRECT_BUFFER,
    /** Indirect command arguments */
    DrawIndirect = GL_DRAW_INDIRECT_BUFFER,
    /** Vertex array indices */
    ElementArray = GL_ELEMENT_ARRAY_BUFFER,
    /** Pixel read target */
    PixelPack = GL_PIXEL_PACK_BUFFER,
    /** Texture data source */
    PixelUnpack = GL_PIXEL_UNPACK_BUFFER,
    /** Query result buffer -- Min version 4.4 */
    Query = GL_QUERY_BUFFER,
    /** Read-write storage for shaders -- Min version 4.3 */
    ShaderStorage = GL_SHADER_STORAGE_BUFFER,
    /** Texture data buffer */
    Texture = GL_TEXTURE_BUFFER,
    /** Transform feedback buffer */
    TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
    /** Uniform block storage */
    Uniform = GL_UNIFORM_BUFFER
};

/**
 * UsageHint is a hint to the GL implementation as to how a buffer object's
 * data store will be accessed.
 * The hint is a suggestion to the GL, but it does not constrain the buffer's
 * actual usage.
 * There are two parts: first the frequency of access, and second the nature
 * of the access.
 * There are three frequencies:
 *   Stream - The data will be modified once and used at most a few times
 *   Static - The data will be modified once and used many times
 *   Dynamic - The data will be modified repeatedly and used many times
 * There are three access types:
 *   Draw - The data is modified by the application and used by the GL
 *   Read - The data is modified by the GL and used by the application
 *   Copy - The data is modified by the GL and used by the GL
 * See documentation for glBufferData for further breakdown.
 */
enum UsageHint
{
    StreamDraw = GL_STREAM_DRAW,
    StreamRead = GL_STREAM_READ,
    StreamCopy = GL_STREAM_COPY,

    StaticDraw = GL_STATIC_DRAW,
    StaticRead = GL_STATIC_READ,
    StaticCopy = GL_STATIC_COPY,

    DynamicDraw = GL_DYNAMIC_DRAW,
    DynamicRead = GL_DYNAMIC_READ,
    DynamicCopy = GL_DYNAMIC_COPY,
};

namespace hidden
{
    template <class T>
    GLenum elementType()
    {
        static_assert(
            std::is_same<GLubyte, T>::value ||
            std::is_same<GLushort, T>::value ||
            std::is_same<GLuint, T>::value,
            "DrawElements only works for buffers which hold "
            "GLubyte (unsigned char), GLushort (unsigned short), or "
            "GLuint (unsigned int");
    }

    template <>
    GLenum elementType<GLubyte>();

    template <>
    GLenum elementType<GLushort>();

    template <>
    GLenum elementType<GLuint>();
}

template <class Data>
class BoundBuffer
{
public:
    BoundBuffer(BindTarget target)
        : target{target}
        , shouldUnbind{true}
    { }

    ~BoundBuffer()
    {
        if (shouldUnbind) {
            glBindBuffer(target, 0);
        }
    }

    BoundBuffer(const BoundBuffer&) = delete;

    BoundBuffer(BoundBuffer&& from)
        : target{from.target}
        , shouldUnbind{from.shouldUnbind}
    {
        from.shouldUnbind = false;
    }

    /**
     * Write data into the GL buffer.
     * Assume the buffer is already bound to the correct target.
     */
    void write(const std::vector<Data>& data,
               UsageHint usage = UsageHint::StreamDraw)
    {
        auto byteSize = data.size() * sizeof(Data);
        glBufferData(target, byteSize, data.data(), usage);
        THROW_ON_GL_ERROR();
    }

    /**
     * Read data out of the GL buffer.
     * The data is undefined if the Data type does not match what is stored in
     * the buffer.
     * Assume the buffer is bound to the correct target.
     * Note to self -- currently this _always_ performs 2 heap allocations.
     * This can definitely be reduced with a better API or some clever caching.
     * @throws GLException
     *     if sizeof(Data)*offset + sizeof(Data)*size is greater
     *     than the size of the GL buffer
     */
    std::vector<Data> read(int size, int offset = 0)
    {
        auto byteOffset = offset * sizeof(Data);
        auto byteSize = size * sizeof(Data);
        auto data = std::unique_ptr<Data[]>{new Data[byteSize]};
        glGetBufferSubData(target, byteOffset, byteSize, data.get());
        THROW_ON_GL_ERROR();

        return std::vector<Data>(data.get(), data.get()+byteSize);
    }

private:
    const BindTarget target;
    bool shouldUnbind;
};

/**
 * This class represents an OpenGL Buffer.
 */
template <class Data>
class Buffer
{
public:
    using Bound = BoundBuffer<Data>;

    /**
     * Create a OpenGL buffer object.
     */
    Buffer()
        : shouldDelete{true}
    {
        glCreateBuffers(1, &handle);
    }

    /**
     * Destroy the OpenGL buffer object.
     */
    ~Buffer()
    {
        if (shouldDelete)
        {
            glDeleteBuffers(1, &handle);
            shouldDelete = false;
        }
    }

    /**
     * The Buffer implementation does not support copying buffer objects.
     */
    Buffer(const Buffer&) = delete;

    /**
     * Transfer ownership of the OpenGL buffer object.
     */
    Buffer(Buffer&& from)
        : shouldDelete{from.shouldDelete}
        , handle{from.handle}
    {
        // don't let the other guy delete our buffer when he goes out of scope!
        from.shouldDelete = false;
    }

    /**
     * Bind the buffer to the last target it was bound to.
     */
    Bound bind(BindTarget target)
    {
        glBindBuffer(target, handle);
        THROW_ON_GL_ERROR();
        return Bound{target};
    }

    /**
     * Get a non-owning reference to the raw OpenGL buffer object.
     */
    GLuint raw() const
    {
        return handle;
    }


    /**
     * Draw's the contents of the buffer using the provided primitive.
     */
    void draw(Primitive primitive, GLuint vertexCount)
    {
        glDrawArrays(primitive, 0, vertexCount);
        THROW_ON_GL_ERROR();
    }

    /**
     * Draw's the contents of the vao using the element buffer.
     */
    void drawElements(Primitive primitive, GLuint vertexCount)
    {
        glDrawElements(GL_LINES, vertexCount, hidden::elementType<Data>(), 0);
        THROW_ON_GL_ERROR();
    }

private:
    bool shouldDelete;
    GLuint handle;
};

} /* namespace NatVis */

#endif
