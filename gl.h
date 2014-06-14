#ifndef RENDER_GL_H_
#define RENDER_GL_H_ 1

#include <functional>

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#if defined(TARGET_OS_IPHONE)
#  include <OpenGLES/ES2/gl.h>
#elif defined(TARGET_PPAPI)
#  include <GLES2/gl2.h>
#else
#  define USING_GLEW 1
#  include <GL/glew.h>
#  include <GL/gl.h>
#endif

namespace render {

// Manages the lifetime of GLuint OpenGL object handles.
class GLHandle {
 public:
  GLHandle() = default;

  GLHandle(GLuint handle, std::function<void(GLuint)> deleter)
      : handle_(handle), deleter_(deleter) {}

  GLHandle(const GLHandle&) = delete;
  GLHandle& operator=(const GLHandle&) = delete;

  GLHandle(GLHandle&& rhs) : handle_(rhs.handle_), deleter_(rhs.deleter_) {
    rhs.handle_ = kInvalidHandle;
  }

  GLHandle& operator=(GLHandle&& rhs) {
    if (handle_ != kInvalidHandle) deleter_(handle_);
    std::swap(deleter_, rhs.deleter_);
    handle_ = rhs.handle_;
    rhs.handle_ = kInvalidHandle;
    return *this;
  }

  ~GLHandle() {
    if (handle_ != kInvalidHandle) deleter_(handle_);
  }

  explicit operator bool() const { return handle_ != kInvalidHandle; }

  GLuint operator*() const { return handle_; }

 private:
  static const GLuint kInvalidHandle = 0;

  GLuint handle_ = kInvalidHandle;
  std::function<void(GLuint)> deleter_;
};

GLHandle CompileShader(const char* source, GLenum type);

GLHandle LinkProgram(const GLHandle& vertex_shader,
                     const GLHandle& fragment_shader);

GLHandle CreateTexture(const void* data, GLsizei width, GLsizei height);

GLHandle CreateBuffer();

}  // namespace render

#endif  // !RENDER_GL_H_
