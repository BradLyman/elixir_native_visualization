#include <AppThread.hpp>
#include <ApplicationWindow.hpp>
#include <ErlContext.hpp>
#include <unordered_map>

using namespace NatVis;
using namespace std;

ErlContext::ErlContext(ErlNifEnv* env)
    : ok{enif_make_atom(env, "ok")}
{ }

ErlContext::~ErlContext()
{ }

ErlContext& ErlContext::in(ErlNifEnv* env)
{
    return *reinterpret_cast<ErlContext*>(enif_priv_data(env));
}

//
// LIFECYCLE METHODS
//

int ErlContext::load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info)
{
    try
    {
        *priv_data = (void*)new ErlContext{env};
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

void ErlContext::unload(ErlNifEnv* env, void* priv_data)
{
    delete (ErlContext*)priv_data;
}

//
// NIF-EXPOSED FUNCTIONS
//

ERL_NIF_TERM ErlContext::startRender(
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

        auto threadId = AppThread::nextId();
        ErlContext::in(env).threads
            .emplace(
                std::piecewise_construct,
                std::forward_as_tuple(threadId),
                std::forward_as_tuple(pid));

        return enif_make_int(env, threadId);
    }
    catch (runtime_error& error) {
        enif_raise_exception(
            env,
            enif_make_string(env, error.what(), ERL_NIF_LATIN1));
    }
    return ErlContext::in(env).ok;
}

ERL_NIF_TERM ErlContext::stopRender(
    ErlNifEnv* env,
    int argc,
    const ERL_NIF_TERM argv[])
{
    int id;
    if (!enif_get_int(env, argv[0], &id)) {
        return enif_make_badarg(env);
    }

    auto& myMap = ErlContext::in(env).threads;
    auto iter = myMap.find(id);
    if (iter != myMap.end()) {
        myMap.erase(iter);
    }

    return ErlContext::in(env).ok;
}

static ErlNifFunc exported_funcs[] = {
    // { name, airity, function }
    {"startRender", 1, ErlContext::startRender},
    {"stopRender", 1, ErlContext::stopRender},
};

ERL_NIF_INIT(
    Elixir.NatVis,
    exported_funcs,
    ErlContext::load,
    NULL,
    NULL,
    ErlContext::unload)

