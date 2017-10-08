#include <ApplicationWindow.hpp>
#include <ErlResourcePtr.hpp>

#include <gl/Program.hpp>
#include <gl/Shader.hpp>

#include <stdexcept>
#include <sstream>

using namespace std;
using namespace NatVis;

namespace
{;

/**
 * Initialize glew to load the various OpenGL function pointers.
 * This should be re-called each time a new context is made current.
 * @throws GLException if there is an error during initialization
 */
void initGlew()
{
    // initialize
    glewExperimental = true;
    auto result = glewInit();
    if (result != GLEW_NO_ERROR)
    {
        throw GLException({ "Failed to initialize GLEW!"
                          , (const char*)glewGetErrorString(result)
                          });
    }

    // A weird bug with GL/Glew will cause a GL_INVALID_ENUM to be raised
    // during initialization. This _shouldn't_ negatively impact the program,
    // so check for the error and swallow the expected here.
    auto glErr = glGetError();
    if (glErr != GL_NO_ERROR && glErr != GL_INVALID_ENUM)
    {
        stringstream ss;
        ss << result;
        throw GLException{ "Unexpected GL error while initializing glew!"
                         , "Error Code " + ss.str()
                         };
    }

    // If there were more errors than just the one.. it's a problem so throw
    THROW_ON_GL_ERROR();
}

} /* namespace */

ErlNifResourceType* ApplicationWindow::erl_type;

ApplicationWindow::ApplicationWindow()
    : isOpen{true}
    , vao{nullptr}
    , vertices{nullptr}
    , sdlWindow{nullptr, CloseWindow{}}
{
   sdlWindow.reset(
        SDL_CreateWindow(
            "tetra-creative",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            640,
            480,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE));

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    context = SDL_GL_CreateContext(sdlWindow.get());
    if (context == NULL) {
        throw new std::runtime_error{"Could not initialize OpenGL Context"};
    }

    SDL_GL_MakeCurrent(sdlWindow.get(), context);
    SDL_GL_SetSwapInterval(1);

    initGlew();

    vertices = make_unique<Buffer<Vertex>>();
    vao = make_unique<Vao>();

    vertices->bind(BindTarget::Array)
        .write(
            { {0.0f, 0.0f}
            , {0.5f, 0.5f}
            , {-0.5f, 0.5f}
            });

    vao->bindLayoutFor<Vertex>()
        .withAttrib(&Vertex::position)
        .toBuffer(*vertices);
}

ApplicationWindow::~ApplicationWindow()
{
    close();
}

void
ApplicationWindow::close() noexcept
{
    // release OpenGL resources before closing the window and deleting the
    // context.
    vertices.reset(nullptr);
    vao.reset(nullptr);

    SDL_GL_DeleteContext(context);
    sdlWindow.reset(nullptr);

    isOpen = false;
}

Status
ApplicationWindow::update()
{
    SDL_GL_MakeCurrent(sdlWindow.get(), context);

    auto stop = false;
    auto event = SDL_Event{};
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return Status::Stop;
        }
        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                int w = (int) event.window.data1;
                int h = (int) event.window.data2;
                glViewport(0, 0, w, h);
            }
        }
    }

    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader vertex = Shader{ShaderType::VERTEX};
    vertex.compile(R"prg(
    #version 130

    in vec2 vVertex;
    void main() {
        gl_Position = vec4(vVertex, 0.0f, 1.0f);
    }
    )prg");

    Shader frag = Shader{ShaderType::FRAGMENT};
    frag.compile(R"prg(
    #version 130

    out vec4 vFragColor;
    void main() {
        vFragColor = vec4(1.0f);
    }
    )prg");

    auto program = ProgramLinker{}
        .vertexAttributes({"vertex"})
        .attach(vertex)
        .attach(frag)
        .link();

    vao->bind();
    program.use();
    THROW_ON_GL_ERROR();

    vertices->draw(Primitive::Triangles, 3);

    SDL_GL_SwapWindow(sdlWindow.get());

    SDL_Delay(5);

    return Status::Continue;
}

void
ApplicationWindow::CloseWindow::operator() (SDL_Window* window)
{
    if (window != nullptr)
    {
        SDL_DestroyWindow(window);
    }
}

