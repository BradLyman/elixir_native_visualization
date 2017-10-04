#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <erl_nif.h>
#include <SDL.h>
#include <SDL_opengl.h>

namespace NatVis
{

struct SDLWindow
{
    SDL_Window* raw;
    SDL_GLContext context;

    /** Erlang resource handle. Populated by Context::load. */
    static ErlNifResourceType* erl_type;
};

/**
 * This object owns all global context for the nif.
 */
class Context
{
private:
    Context(ErlNifEnv* env);
    ~Context();
    Context(const Context&) = delete;
    Context(Context&&) = delete;

public:
    const ERL_NIF_TERM ok;
    const ERL_NIF_TERM stop;

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
