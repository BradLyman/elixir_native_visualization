#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <erl_nif.h>
#include <AppThread.hpp>
#include <unordered_map>

namespace NatVis
{;

/**
 * This object owns all global context for the nif.
 */
class ErlContext
{
private:
    ErlContext(ErlNifEnv* env);
    ~ErlContext();
    ErlContext(const ErlContext&) = delete;
    ErlContext(ErlContext&&) = delete;

public:
    const ERL_NIF_TERM ok;
    std::unordered_map<int, AppThread> threads;

public:
    /**
     * Get a reference to the context associated with the current environment.
     */
    static ErlContext& in(ErlNifEnv* env);

    //
    // LIFECYCLE METHODS
    //

    /**
     * Load the NIF.
     */
    static int load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info);

    /**
     * Unload the NIF.
     */
    static void unload(ErlNifEnv* env, void* priv_data);

    //
    // NIF-EXPOSED FUNCTIONS
    //

    /**
     * Start a render window and associated resources.
     */
    static ERL_NIF_TERM startRender(
         ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);

    /**
     * Stop rendering and free resources associated with the renderer.
     */
    static ERL_NIF_TERM stopRender(
        ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
};

} /* namespace NatVis */

#endif
