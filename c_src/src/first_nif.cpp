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

namespace
{
    void* thread_main(void* rawAppThread) {
        auto* appThread = reinterpret_cast<AppThread*>(rawAppThread);
        appThread->threadMain();
        return nullptr;
    }
}

AppThread::AppThread()
    : running{true}
{
    char threadName[] = "thread opts";
    auto* opts = enif_thread_opts_create(threadName);

    auto result = enif_thread_create(
        threadName,
        &this->erlThread,
        &thread_main,
        reinterpret_cast<void*>(this),
        opts);

    enif_thread_opts_destroy(opts);

    if (result != 0) {
        throw std::runtime_error{
            "Failed to create Erlang thread! Error code: " + to_string(result)};
    }
}

AppThread::~AppThread() {
    stop();
    enif_thread_join(this->erlThread, nullptr);
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
    try
    {
        return ErlResourcePtr<AppThread>{}.asTerm(env);
    }
    catch (runtime_error& error) {
        enif_raise_exception(
            env,
            enif_make_string(env, error.what(), ERL_NIF_LATIN1));
    }
    return Context::in(env).ok;
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

