#include "Context.hpp"

#include <erl_nif.h>
#include <SDL.h>

#include <iostream>

using NatVis::Context;

namespace
{;

ERL_NIF_TERM open_window(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    NatVis::SDLWindow* window =
        reinterpret_cast<NatVis::SDLWindow*>(
            enif_alloc_resource(
                &Context::in(env).SDLWindow,
                sizeof(NatVis::SDLWindow)));

    window->raw = SDL_CreateWindow(
        "NatVis",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    ERL_NIF_TERM window_term = enif_make_resource(env, (void*)window);
    enif_release_resource((void*)window);

    return window_term;
}

ERL_NIF_TERM close_window(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    NatVis::SDLWindow* window;
    auto res = enif_get_resource(env, argv[0], &Context::in(env).SDLWindow, (void**)&window);
    if (!res)
    {
        std::cout << "Could not get resource type!" << std::endl;
    }

    SDL_DestroyWindow(window->raw);
    window->raw = nullptr;

    return Context::in(env).ok;
}


ErlNifFunc exported_funcs[] = {
    // { name, airity, function }
    {"open_window", 0, open_window},
    {"close_window", 1, close_window},
};

}

ERL_NIF_INIT(
    Elixir.NatVis,
    exported_funcs,
    Context::load,
    NULL,
    NULL,
    Context::unload)

