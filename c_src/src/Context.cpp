#include "Context.hpp"

#include <iostream>

using NatVis::Context;
using NatVis::SDLWindow;

ErlNifResourceType* SDLWindow::erl_type;

Context::Context(ErlNifEnv* env)
    : ok{enif_make_atom(env, "ok")}
    , stop{enif_make_atom(env, "stop")}
{
    auto resultCode = SDL_Init(SDL_INIT_EVERYTHING);
    if (resultCode < 0)
    {
        throw std::runtime_error{SDL_GetError()};
    }
    std::cout << "SDL_Init success!" << std::endl;
};

Context::~Context()
{
    SDL_Quit();
};

Context& Context::in(ErlNifEnv* env)
{
    return *reinterpret_cast<Context*>(enif_priv_data(env));
}

static void dtor(ErlNifEnv* env, void* resource) {
    NatVis::SDLWindow* window = (NatVis::SDLWindow*)resource;
    if (window->raw != nullptr)
    {
        SDL_GL_DeleteContext(window->context);
        SDL_DestroyWindow(window->raw);
        window->raw = nullptr;
    }
}

int Context::load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info)
{
    try
    {
        ErlNifResourceFlags flags =
            (ErlNifResourceFlags)(ERL_NIF_RT_TAKEOVER | ERL_NIF_RT_CREATE);
        ErlNifResourceFlags tried;
        SDLWindow::erl_type =
            enif_open_resource_type(
                env,
                "NatVis",
                "SDLWindow",
                dtor,
                flags,
                &tried);

        *priv_data = (void*)new Context{env};
    }
    catch (std::exception& ex)
    {
        std::cout
            << "Failed to initialize context! "
            << ex.what()
            << std::endl;
        return 1;
    }
    std::cout << "Context initialized" << std::endl;
    return 0;
}

void Context::unload(ErlNifEnv* env, void* priv_data)
{
    delete (Context*)priv_data;
}

