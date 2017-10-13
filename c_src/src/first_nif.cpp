#include <ApplicationWindow.hpp>
#include <AppThread.hpp>
#include <ErlResourcePtr.hpp>
#include <Context.hpp>

#include <erl_nif.h>
#include <SDL.h>

#include <iostream>

using namespace NatVis;
using namespace std;

ErlNifResourceType* AppThread::erl_type = nullptr;

AppThread::AppThread()
    : running{true}
{
    app = std::thread{[this]{
        this->threadMain();
    }};
}

AppThread::~AppThread() {
    stop();
    app.join();
}

void
AppThread::threadMain() {
    auto window = ApplicationWindow{};
    while (running && window.update() == Status::Continue)
    { }
}

void
AppThread::stop() {
    running = false;
}

static ERL_NIF_TERM start(
    ErlNifEnv* env,
    int argc,
    const ERL_NIF_TERM argv[])
{
    return ErlResourcePtr<AppThread>{}.asTerm(env);
}

static ERL_NIF_TERM stop(
    ErlNifEnv* env,
    int argc,
    const ERL_NIF_TERM argv[])
{
    try
    {
        auto app = ErlResourcePtr<AppThread>::fromTerm(env, argv[0]);
        app->stop();
    }
    catch (InvalidResource&)
    {
        return enif_make_badarg(env);
    }
    return Context::in(env).ok;
}

static ErlNifFunc exported_funcs[] = {
    // { name, airity, function }
    {"start", 0, start},
    {"stop", 1, stop},
};

ERL_NIF_INIT(
    Elixir.NatVis,
    exported_funcs,
    Context::load,
    NULL,
    NULL,
    Context::unload)

