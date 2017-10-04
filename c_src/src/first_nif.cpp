#include <GL/glew.h>

#include <ErlResourcePtr.hpp>
#include "Context.hpp"

#include <erl_nif.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <iostream>

using NatVis::Context;

static ERL_NIF_TERM open_window(
    ErlNifEnv* env,
    int argc,
    const ERL_NIF_TERM argv[])
{
    auto window = ErlResourcePtr<NatVis::SDLWindow>{};

    window->raw = SDL_CreateWindow(
        "NatVis",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window->context = SDL_GL_CreateContext(window->raw);
    if (window->context == NULL) {
        enif_raise_exception(
            env,
            enif_make_string(
                env,
                "could not create opengl context",
                ERL_NIF_LATIN1));
    }

    SDL_GL_MakeCurrent(window->raw, window->context);
    SDL_GL_SetSwapInterval(1);

    auto result = glewInit();
    if (result != GLEW_OK) {
        enif_raise_exception(
            env,
            enif_make_string(
                env,
                "could not initialize glew",
                ERL_NIF_LATIN1));
    }

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
        std::cout << "Got " << i << std::endl;

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

