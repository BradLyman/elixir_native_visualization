#ifndef APP_THREAD_HPP
#define APP_THREAD_HPP

#include <ApplicationWindow.hpp>

#include <erl_nif.h>

#include <atomic>
#include <thread>

namespace NatVis
{

class AppThread {
public:
    AppThread(ErlNifPid eventHandler);
    ~AppThread();

    AppThread(const AppThread&) = delete;
    AppThread& operator=(const AppThread&) = delete;

    AppThread(AppThread&&) = default;
    AppThread& operator=(AppThread&&) = default;

    void threadMain();

    void stop();

    static ErlNifResourceType* erl_type;

private:
    std::atomic<bool> running;
    ErlNifTid erlThread;
    ErlNifPid eventHandler;
};

} /* namespace NatVis */

#endif
