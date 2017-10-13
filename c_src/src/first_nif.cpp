#include <ApplicationWindow.hpp>
#include <ErlResourcePtr.hpp>
#include <Context.hpp>

#include <erl_nif.h>
#include <SDL.h>

#include <iostream>

using namespace NatVis;
using namespace std;

static ERL_NIF_TERM open_window(
    ErlNifEnv* env,
    int argc,
    const ERL_NIF_TERM argv[])
{
    return ErlResourcePtr<ApplicationWindow>{}.asTerm(env);
}

static ERL_NIF_TERM close_window(
    ErlNifEnv* env,
    int argc,
    const ERL_NIF_TERM argv[])
{
    try
    {
        auto window = ErlResourcePtr<ApplicationWindow>::fromTerm(env, argv[0]);
        window->close();
    }
    catch (InvalidResource&)
    {
        return enif_make_badarg(env);
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

        auto window = ErlResourcePtr<ApplicationWindow>::fromTerm(env, argv[1]);
        enif_consume_timeslice(env, 100);
        switch (window->update())
        {
            case Status::Continue:
                return Context::in(env).ok;

            case Status::Stop:
                return Context::in(env).stop;
        }
    }
    catch (InvalidResource&)
    {
        return enif_make_badarg(env);
    }
    catch (std::exception& ex)
    {
        enif_raise_exception(
            env, enif_make_string(env, ex.what(), ERL_NIF_LATIN1));
        return Context::in(env).stop;
    }
}

static ErlNifFunc exported_funcs[] = {
    // { name, airity, function }
    {"update", 2, update, ERL_NIF_DIRTY_JOB_CPU_BOUND},
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

