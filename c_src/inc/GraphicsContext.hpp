#ifndef GRAPHICS_CONTEXT_HPP
#define GRAPHICS_CONTEXT_HPP

#include <gl/VAO.hpp>
#include <gl/Buffer.hpp>
#include <gl/Program.hpp>

#include <vector>

namespace NatVis
{

struct Vertex
{
    std::array<float, 3> position;
};

/**
 * The graphics context is responsible for owning all resources which are
 * tied to the openGL context.
 * The GraphicsContext is constructed when a openGL context is bound and
 * current, so resources can be manipulated safely in the constructor.
 */
class GraphicsContext
{
public:
    GraphicsContext();
    ~GraphicsContext();

    /**
     * Render a single frame to the screen.
     */
    void renderFrame();

private:
    Vao pointsArrayObject;
    Buffer<Vertex> pointsBuffer;
    Program sceneProgram;
    std::vector<Vertex> points;
};

} /* namespace NatVis */

#endif

