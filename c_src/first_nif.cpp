#include <erl_nif.h>
#include <SDL.h>

#include <iostream>

class Context
{
private:
    Context(ErlNifEnv* env)
        : ok{enif_make_atom(env, "ok")}
    {
        auto resultCode = SDL_Init(SDL_INIT_EVERYTHING);
        if (resultCode < 0)
        {
            throw std::runtime_error{SDL_GetError()};
        }
        std::cout << "SDL_Init success!" << std::endl;
    }

    ~Context()
    {
        SDL_Quit();
    };

    Context(const Context&) = delete;
    Context(Context&&) = delete;

public:
    const ERL_NIF_TERM ok;

public:
    /**
     * Get a reference to the context associated with the current environment.
     */
    static Context&
    in(ErlNifEnv* env)
    {
        return *reinterpret_cast<Context*>(enif_priv_data(env));
    }

    /**
     * Passed to ERL_NIF_INIT to load the context as private data.
     */
    static int
    load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info)
    {
        try
        {
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

    /**
     * Passed to ERL_NIF_INIT to delete the context stored in private data.
     */
    static void
    unload(ErlNifEnv* env, void* priv_data)
    {
        delete (Context*)priv_data;
    }
};

static ERL_NIF_TERM
open_window(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    SDL_Window* window = SDL_CreateWindow(
        "NatVis",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    SDL_Delay(1000);

    SDL_DestroyWindow(window);

    return Context::in(env).ok;
}


static ErlNifFunc exported_funcs[] = {
    // { name, airity, function }
    {"open_window", 0, open_window},
};

ERL_NIF_INIT(
    Elixir.NatVis,
    exported_funcs,
    Context::load,
    NULL,
    NULL,
    Context::unload)

