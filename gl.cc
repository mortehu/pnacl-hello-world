#include <stdio.h>
#include <string.h>

#include "gl.h"

namespace render {

GLHandle CompileShader(const char* string, GLenum type) {
  GLHandle result(glCreateShader(type), glDeleteShader);

  GLint string_length = strlen(string);
  glShaderSource(*result, 1, &string, &string_length);
  glCompileShader(*result);

  GLint compile_status;
  glGetShaderiv(*result, GL_COMPILE_STATUS, &compile_status);

  if (compile_status != GL_TRUE) {
    GLchar log[1024];
    GLsizei log_length;
    glGetShaderInfoLog(*result, sizeof(log), &log_length, log);

    fprintf(stderr, "glCompileShader failed: %.*s\n",
            static_cast<int>(log_length), log);
    return GLHandle();
  }

  return result;
}

GLHandle LinkProgram(const GLHandle& vertex_shader,
                     const GLHandle& fragment_shader) {
  GLHandle handle(glCreateProgram(), glDeleteProgram);

  glAttachShader(*handle, *vertex_shader);
  glAttachShader(*handle, *fragment_shader);
  glLinkProgram(*handle);

  GLint link_status;
  glGetProgramiv(*handle, GL_LINK_STATUS, &link_status);

  if (link_status != GL_TRUE) {
    GLchar log[1024];
    GLsizei log_length;
    glGetProgramInfoLog(*handle, sizeof(log), &log_length, log);

    fprintf(stderr, "glLinkProgram failed: %.*s\n",
            static_cast<int>(log_length), log);
    return GLHandle();
  }

  glUseProgram(*handle);

  return handle;
}

GLHandle CreateTexture(const void* data, GLsizei width, GLsizei height) {
  GLuint handle;
  glGenTextures(1, &handle);
  GLHandle result(handle, [](GLuint handle) { glDeleteTextures(1, &handle); });

  glBindTexture(GL_TEXTURE_2D, *result);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  return result;
}

GLHandle CreateBuffer() {
  GLuint handle;
  glGenBuffers(1, &handle);
  return GLHandle(handle, [](GLuint handle) { glDeleteBuffers(1, &handle); });
}

}  // namespace render
