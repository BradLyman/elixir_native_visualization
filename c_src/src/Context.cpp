#include <AppThread.hpp>
#include <ApplicationWindow.hpp>
#include <Context.hpp>
#include <unordered_map>

using namespace NatVis;
using namespace std;

Context::Context(ErlNifEnv* env)
    : ok{enif_make_atom(env, "ok")}
    , stop{enif_make_atom(env, "stop")}
{ }

Context::~Context()
{ }

Context& Context::in(ErlNifEnv* env)
{
    return *reinterpret_cast<Context*>(enif_priv_data(env));
}

template <class T>
static void destroy(ErlNifEnv* env, void* resource) {
    T* typed = (T*)resource;
    typed->~T(); // manually call the destructor
}

int Context::load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info)
{
    try
    {
        ErlNifResourceFlags flags =
            (ErlNifResourceFlags)(ERL_NIF_RT_TAKEOVER | ERL_NIF_RT_CREATE);
        ErlNifResourceFlags tried;
        ApplicationWindow::erl_type =
            enif_open_resource_type(
                env,
                "NatVis",
                "ApplicationWindow",
                destroy<ApplicationWindow>,
                flags,
                &tried);

        AppThread::erl_type =
            enif_open_resource_type(
                env,
                "NatVis",
                "AppThread",
                destroy<AppThread>,
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

