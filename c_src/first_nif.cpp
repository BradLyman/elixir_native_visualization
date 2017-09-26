#include <erl_nif.h>
#include <SDL.h>

static ERL_NIF_TERM
open_window(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow(
        "mywindow",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_SHOWN);
    SDL_Delay(2000);

    SDL_DestroyWindow(window);

    SDL_Quit();

    return enif_make_atom(env, "ok");
}

static ErlNifFunc exported_funcs[] = {
    // { name, airity, function }
    {"open_window", 0, open_window},
};

ERL_NIF_INIT(Elixir.NatVis, exported_funcs, NULL, NULL, NULL, NULL)

