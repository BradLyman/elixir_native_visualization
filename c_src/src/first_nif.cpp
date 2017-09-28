#include <ErlResourcePtr.hpp>
#include "Context.hpp"

#include <erl_nif.h>
#include <SDL.h>

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
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

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

static ErlNifFunc exported_funcs[] = {
    // { name, airity, function }
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

