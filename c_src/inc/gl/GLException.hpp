#ifndef GLEXCEPTION_HPP
#define GLEXCEPTION_HPP

#include <exception>
#include <initializer_list>
#include <vector>
#include <string>

namespace NatVis
{;

/**
 * Throw a GLException if there is an error in the GL.
 * The result of glGetError is added to the messages like so:
 * GLException: glGetError -- $error string$ -- $messages$
 */
void onGlError(std::string file, std::string line);

/**
 * This class is used to represent exceptions while working with OpenGL.
 */
class GLException : public std::exception
{
public:
    /**
     * Construct an exception with each of the messages joined with spaces.
     */
    GLException(std::vector<std::string> messages);
    GLException(std::initializer_list<std::string> messages);

    /**
     * Get the complete exception mesage.
     */
    const char* what() const noexcept override;

private:
    std::string errorMsg;
};

}; /* namespace NatVis */


#define _EXPAND_GL_ERR_IMPL(file, line) \
    NatVis::onGlError(#file, #line);

#define _EXPAND_GL_ERR(file, line) \
    _EXPAND_GL_ERR_IMPL(file, line)

#define THROW_ON_GL_ERROR() \
    _EXPAND_GL_ERR(__FILE__, __LINE__)

#endif
