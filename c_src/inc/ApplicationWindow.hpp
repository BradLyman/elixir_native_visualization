#ifndef APPLICATION_WINDOW_HPP
#define APPLICATION_WINDOW_HPP

#include <gl/Buffer.hpp>
#include <gl/VAO.hpp>
#include <erl_nif.h>

#include <SDL.h>
#include <memory>

namespace NatVis
{;

struct Vertex
{
    std::array<float, 2> position;
};

enum class Status
{
    Continue,
    Stop
};

class ApplicationWindow
{
    struct CloseWindow
    {
        void operator() (SDL_Window* window);
    };
    using RawWindow = std::unique_ptr<SDL_Window, CloseWindow>;

public:
    ApplicationWindow();
    ~ApplicationWindow();

    void close() noexcept;

    Status update();

    std::unique_ptr<Vao> vao;
    std::unique_ptr<Buffer<Vertex>> vertices;
    SDL_GLContext context;
    RawWindow sdlWindow;

    /** Erlang resource handle. Populated by Context::load. */
    static ErlNifResourceType* erl_type;

private:
    bool isOpen;
};

} /* namespace NatVis */

#endif
