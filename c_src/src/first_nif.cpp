#include <ApplicationWindow.hpp>
#include <AppThread.hpp>
#include <ErlResourcePtr.hpp>
#include <Context.hpp>

#include <erl_nif.h>
#include <SDL.h>

#include <iostream>
#include <unordered_map>

using namespace NatVis;
using namespace std;

ErlNifResourceType* AppThread::erl_type = nullptr;

namespace
{
    void* thread_main(void* rawAppThread) {
        auto* appThread = reinterpret_cast<AppThread*>(rawAppThread);
        appThread->threadMain();
        cout << "return from threadmain" << endl;
        return nullptr;
    }

    int appId = 0;
}

AppThread::AppThread(ErlNifPid eventHandler)
    : running{true}
    , eventHandler{eventHandler}
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
    cout << "ending appthread" << endl;
    stop();
    enif_thread_join(this->erlThread, nullptr);
    cout << "join complete" << endl;
}

void
AppThread::threadMain() {
    auto window = ApplicationWindow{eventHandler};
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
        ErlNifPid pid;
        auto result = enif_get_local_pid(env, argv[0], &pid);
        if (result == false) {
            throw std::runtime_error{"Could not find pid!"};
        }

        Context::in(env).threads
            .emplace(
                std::piecewise_construct,
                std::forward_as_tuple(appId),
                std::forward_as_tuple(pid));

        return enif_make_int(env, appId++);
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
    auto& myMap = Context::in(env).threads;

    int id;
    enif_get_int(env, argv[0], &id);

    auto iter = myMap.find(id);

    if (iter != myMap.end()) {
        myMap.erase(iter);
    }
    return Context::in(env).ok;
}

static ErlNifFunc exported_funcs[] = {
    // { name, airity, function }
    {"start", 1, start},
    {"stop", 1, stop},
};

ERL_NIF_INIT(
    Elixir.NatVis,
    exported_funcs,
    Context::load,
    NULL,
    NULL,
    Context::unload)

