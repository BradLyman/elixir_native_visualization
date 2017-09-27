#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <erl_nif.h>
#include <SDL.h>

namespace NatVis
{

struct SDLWindow
{
    SDL_Window* raw;
};

/**
 * This object owns all global context for the nif.
 */
class Context
{
private:
    Context(ErlNifEnv* env, ErlNifResourceType* ErlSdlWindow);
    ~Context();
    Context(const Context&) = delete;
    Context(Context&&) = delete;

public:
    const ERL_NIF_TERM ok;

    ErlNifResourceType& SDLWindow;

public:
    /**
     * Get a reference to the context associated with the current environment.
     */
    static Context& in(ErlNifEnv* env);

    /**
     * Passed to ERL_NIF_INIT to load the context as private data.
     */
    static int load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info);

    /**
     * Passed to ERL_NIF_INIT to delete the context stored in private data.
     */
    static void unload(ErlNifEnv* env, void* priv_data);
};

} /* namespace NatVis */

#endif
