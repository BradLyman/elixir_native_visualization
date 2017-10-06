#include <gl/GLException.hpp>

#include <GL/glew.h>

#include <sstream>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace NatVis;

namespace
{
    struct HumanReadable
    {
        string error;
        string description;

        HumanReadable() = default;
    };

    HumanReadable humanReadable(GLenum error)
    {
#define errcase(err, msg) \
    case err:             \
        return {          \
            #err,         \
            msg           \
        };

    switch (error)
    {
errcase(GL_NO_ERROR, R"msg(
    No error has been recorded.
    The value of this symbolic constant is guaranteed to be 0
)msg"
)
errcase(GL_INVALID_ENUM, R"msg(
   An unacceptable value is specified for an enumerated argument. The offending
   command is ignored and has no other side effect than to set the error flag.
)msg"
)
errcase(GL_INVALID_VALUE, R"msg(
    A numeric argument is out of range. The offending command is ignored and has no
    other side effect than to set the error flag.
)msg"
)
errcase(GL_INVALID_OPERATION, R"msg(
    The specified operation is not allowed in the current state. The offending
    command is ignored and has no other side effect than to set the error flag.
)msg"
)
errcase(GL_INVALID_FRAMEBUFFER_OPERATION, R"msg(
    The framebuffer object is not complete. The offending command is ignored and has
    no other side effect than to set the error flag.
)msg"
)
errcase(GL_OUT_OF_MEMORY, R"msg(
    There is not enough memory left to execute the command. The state of the GL is
    undefined, except for the state of the error flags, after this error is
    recorded.
)msg"
)
errcase(GL_STACK_UNDERFLOW, R"msg(
    An attempt has been made to perform an operation that would cause an internal
    stack to underflow.
)msg"
)
errcase(GL_STACK_OVERFLOW, R"msg(
    An attempt has been made to perform an operation that would cause an internal
    stack to overflow.
)msg"
)
default:
    stringstream ss;
    ss << error;
    return { "Unknown GL_ERROR"
           , ss.str()
           };
        }
#undef errcase
    }
}

void
NatVis::onGlError(string file, string line)
{
    auto humanErrors = vector<HumanReadable>{};
    auto rawError = glGetError();
    while (rawError != GL_NO_ERROR)
    {
        humanErrors.push_back(humanReadable(rawError));
        rawError = glGetError();
    }

    if (humanErrors.size() != 0)
    {
        auto messages = vector<string>{ file
                                      , "[" + line + "]\n"
                                      };
        for (auto humanError : humanErrors)
        {
            messages.push_back(humanError.error);
            messages.push_back(humanError.description);
        }
        throw GLException{messages};
    }
}

GLException::GLException(vector<string> messages)
{
    errorMsg = accumulate(
        begin(messages), end(messages),
        string("GLException:"),
        [](string fullMsg, string curMsg)
        {
            return fullMsg + " " + curMsg;
        });
}

GLException::GLException(initializer_list<string> messages)
    : GLException(vector<string>{messages})
{ }

const char*
GLException::what() const noexcept
{
    return errorMsg.c_str();
}

