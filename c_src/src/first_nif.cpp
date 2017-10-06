#include <GL/glew.h>

#include <ErlResourcePtr.hpp>
#include "Context.hpp"
#include <gl/Shader.hpp>
#include <gl/Program.hpp>

#include <erl_nif.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <iostream>
#include <sstream>

using namespace NatVis;
using namespace std;

namespace {
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

};

static ERL_NIF_TERM open_window(
    ErlNifEnv* env,
    int argc,
    const ERL_NIF_TERM argv[])
{

    SDL_Window* rawWindow = SDL_CreateWindow(
        "tetra-creative",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GLContext rawContext = SDL_GL_CreateContext(rawWindow);
    if (rawContext == NULL) {
        enif_raise_exception(
            env,
            enif_make_string(
                env,
                "could not create opengl context",
                ERL_NIF_LATIN1));
    }

    SDL_GL_MakeCurrent(rawWindow, rawContext);
    SDL_GL_SetSwapInterval(1);

    initGlew();

    auto window = ErlResourcePtr<NatVis::SDLWindow>{};

    window->raw = rawWindow;
    window->context = rawContext;
    window->vertices
        .bind(BindTarget::Array)
        .write({ {0.0f, 0.0f}
               , {0.5f, 0.5f}
               , {-0.5f, 0.5f}
               });

    window->vao
        .bindLayoutFor<Vertex>()
        .withAttrib(&Vertex::position)
        .toBuffer(window->vertices);


    return window.asTerm(env);
}

static ERL_NIF_TERM close_window(
    ErlNifEnv* env,
    int argc,
    const ERL_NIF_TERM argv[])
{
    try
    {
        auto window =
            ErlResourcePtr<NatVis::SDLWindow>::fromTerm(env, argv[0]);

        SDL_GL_DeleteContext(window->context);
        SDL_DestroyWindow(window->raw);
        window->raw = nullptr;
    }
    catch (std::exception& ex)
    {
        enif_raise_exception(
            env, enif_make_string(env, ex.what(), ERL_NIF_LATIN1));
    }

    return Context::in(env).ok;
}

static ERL_NIF_TERM update(
    ErlNifEnv* env,
    int argc,
    const ERL_NIF_TERM argv[])
{
    try
    {
        int i;
        enif_get_int(env, argv[0], &i);

        auto window =
            ErlResourcePtr<NatVis::SDLWindow>::fromTerm(env, argv[1]);

        SDL_GL_MakeCurrent(window->raw, window->context);

        auto stop = false;
        auto event = SDL_Event{};
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                stop = true;
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

        program.use();
        window->vao.bind();

        window->vertices.draw(Primitive::Triangles, 3);

        SDL_GL_SwapWindow(window->raw);

        SDL_Delay(5);

        if (stop)
        {
            return Context::in(env).stop;
        }
    }
    catch (std::exception& ex)
    {
        enif_raise_exception(
            env, enif_make_string(env, ex.what(), ERL_NIF_LATIN1));
    }

    return Context::in(env).ok;
}

static ErlNifFunc exported_funcs[] = {
    // { name, airity, function }
    {"update", 2, update},
    {"open_window", 0, open_window},
    {"close_window", 1, close_window},
};

ERL_NIF_INIT(
    Elixir.NatVis,
    exported_funcs,
    Context::load,
    NULL,
    NULL,
    Context::unload)

