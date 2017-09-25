#include <erl_nif.h>

static ERL_NIF_TERM
open_window(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    return enif_make_atom(env, "ok");
}

static ErlNifFunc exported_funcs[] = {
    // { name, airity, function }
    {"open_window", 0, open_window},
};

ERL_NIF_INIT(Elixir.NatVis, exported_funcs, NULL, NULL, NULL, NULL)

