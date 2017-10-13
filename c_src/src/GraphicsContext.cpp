#include <GraphicsContext.hpp>
#include <gl/Shader.hpp>
#include <gl/Program.hpp>

#include <cmath>

using namespace NatVis;
using namespace std;

namespace
{
const char* IDENTITY_VERTEX_SRC = R"prg(
    #version 130

    in vec3 vVertex;
    void main() {
        gl_Position = vec4(vVertex, 1.0f);
    }
)prg";

const char* ALWAYS_WHITE_FRAG_SRC = R"prg(
    #version 130

    out vec4 vFragColor;
    void main() {
        vFragColor = vec4(1.0f);
    }
)prg";

void addPointsInCircle(vector<Vertex>& points, int count)
{
    static auto offset = 0.0f;

    offset += 0.005f;

    points.clear();
    auto angle = 6.28f / count;
    for (int i = 0; i < count; i++) {
        points.push_back({
            cos(angle*i+offset), sin(angle*i+offset), 0.0f
        });
    }
}
}

GraphicsContext::GraphicsContext()
{
    pointsBuffer
        .bind(BindTarget::Array)
        .write(
            { {0.0f, 0.0f, 0.0f}
            , {0.5f, 0.5f, 0.0f}
            , {-0.5f, 0.5f, 0.0f}
            });

    Shader vertex = Shader{ShaderType::VERTEX};
    Shader frag = Shader{ShaderType::FRAGMENT};

    vertex.compile(IDENTITY_VERTEX_SRC);
    frag.compile(ALWAYS_WHITE_FRAG_SRC);

    sceneProgram
        .configRenderPipeline()
        .withAttribute("vVertex")
        .withShader(vertex)
        .withShader(frag)
        .link();

    pointsArrayObject
        .bindLayoutFor<Vertex>()
        .withAttrib(&Vertex::position)
        .toBuffer(pointsBuffer);
    addPointsInCircle(points, 50);
}

GraphicsContext::~GraphicsContext() { }

void
GraphicsContext::renderFrame()
{
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    pointsArrayObject.bind();
    sceneProgram.use();

    pointsBuffer
        .bind(BindTarget::Array)
        .write(points);

    pointsBuffer.draw(Primitive::Points, points.size());
}

