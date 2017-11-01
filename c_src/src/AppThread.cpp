#include <AppThread.hpp>
#include <iostream>

using namespace NatVis;
using namespace std;

namespace
{
    void* thread_main(void* rawAppThread) {
        auto* appThread = reinterpret_cast<AppThread*>(rawAppThread);
        appThread->threadMain();
        return nullptr;
    }
}

int AppThread::nextId()
{
    static int counter = 0;
    return counter++;
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
    stop();
    enif_thread_join(this->erlThread, nullptr);
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

