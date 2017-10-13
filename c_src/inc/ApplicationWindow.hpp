#ifndef APPLICATION_WINDOW_HPP
#define APPLICATION_WINDOW_HPP

#include <GraphicsContext.hpp>

#include <gl/Buffer.hpp>
#include <gl/VAO.hpp>
#include <erl_nif.h>

#include <SDL.h>
#include <memory>
#include <chrono>

namespace NatVis
{;

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

    SDL_GLContext context;
    RawWindow sdlWindow;
    std::unique_ptr<GraphicsContext> graphicsContext;

    /** Erlang resource handle. Populated by Context::load. */
    static ErlNifResourceType* erl_type;

private:
    using Timestamp = std::chrono::high_resolution_clock::time_point;
    bool isOpen;
};

} /* namespace NatVis */

#endif
