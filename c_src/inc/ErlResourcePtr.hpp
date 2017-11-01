#ifndef ERL_RESOURCE_PTR_HPP
#define ERL_RESOURCE_PTR_HPP

#include <erl_nif.h>
#include <exception>

class InvalidResource : public std::exception { };

/**
 * This class is responsible for holding an Erlang resource.
 * By convention resource types have a static member called "erl_type" which
 * should be initialized by the Context::load and is used to identify the type
 * to erlang.
 */
template <class Res>
class ErlResourcePtr
{
public:
    /**
     * Allocate a new resource instance.
     */
    template <typename ...Args>
    static ErlResourcePtr<Res> build(Args&&... args) {
        Res* resource = reinterpret_cast<Res*>(
            enif_alloc_resource(Res::erl_type, sizeof(Res)));
        new ((void*)resource) Res(std::forward<Args...>(args...));
        return ErlResourcePtr<Res>(resource);
    }

private:
    ErlResourcePtr(Res* toOwn)
        : resource{toOwn}
    { }

public:

    /**
     * Add a reference to the provided resource to maintain ownership.
     */
    ErlResourcePtr(void* ptr)
    {
        enif_keep_resource(ptr);
        resource = reinterpret_cast<Res*>(ptr);
    }

    /**
     * Release the reference to the resource.
     */
    ~ErlResourcePtr()
    {
        if (resource != nullptr)
        {
            enif_release_resource(reinterpret_cast<void*>(resource));
        }
    }

    /**
     * Move the resource into a new owning ptr.
     */
    ErlResourcePtr(ErlResourcePtr&& from)
    {
        this->resource = from.resource;
        from.resource = nullptr;
    }

    /**
     * Resources do not support being copied.
     */
    ErlResourcePtr(const ErlResourcePtr&) = delete;

    /**
     * Build a native Erlang  term using this resource.
     */
    ERL_NIF_TERM asTerm(ErlNifEnv* env)
    {
        return enif_make_resource(env, reinterpret_cast<void*>(resource));
    }

    /** dereference operator */
    Res& operator*()
    {
        return &this->resource;
    }

    /** dereference operator */
    const Res& operator*() const
    {
        return &this->resource;
    }

    /** dereference operator */
    Res* operator->()
    {
        return this->resource;
    }

    /** dereference operator */
    const Res* operator->() const
    {
        return this->resource;
    }

    /**
     * Get an owning resource ptr from the provided term.
     * @throws std::runtime_error if getting the resource fails.
     */
    static ErlResourcePtr<Res> fromTerm(ErlNifEnv* env, const ERL_NIF_TERM term)
    {
        Res* raw;
        auto result = enif_get_resource(env, term, Res::erl_type, (void**)&raw);
        if (!result)
        {
            throw InvalidResource{};
        }

        return ErlResourcePtr<Res>{reinterpret_cast<Res*>(raw)};
    }

private:
    Res* resource = nullptr;
};

#endif
