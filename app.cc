#include <memory>

#include "app.h"
#include "gl.h"
#include "vertex.h"

namespace app {

namespace {

const char* kVertexShaderSource =
    "attribute vec2 attr_VertexPosition;\n"
    "attribute vec2 attr_TextureCoord;\n"
    "uniform vec2 uniform_RcpWindowSize;\n"
    "varying vec2 var_TextureCoord;\n"
    "void main (void)\n"
    "{\n"
    "  gl_Position = vec4(-1.0 + (attr_VertexPosition.x * "
    "uniform_RcpWindowSize.x) * 2.0,\n"
    "                      1.0 - (attr_VertexPosition.y * "
    "uniform_RcpWindowSize.y) * 2.0, 0.0, 1.0);\n"
    "  var_TextureCoord = attr_TextureCoord;\n"
    "}";

const char* kFragmentShaderSource =
    "precision mediump float;\n"
    "varying vec2 var_TextureCoord;\n"
    "uniform sampler2D uniform_Sampler;\n"
    "void main (void)\n"
    "{\n"
    "  gl_FragColor = texture2D(uniform_Sampler, vec2 (var_TextureCoord.s, "
    "var_TextureCoord.t));\n"
    "}";


render::GLHandle quad_buffer_;
render::GLHandle texture_;
render::GLHandle shader_;

render::GLHandle CreateGradientTexture(unsigned int width,
                                       unsigned int height) {
  std::unique_ptr<uint8_t[]> buffer(new uint8_t[width * height * 4]);

  for (unsigned int y = 0, i = 0; y < height; ++y) {
    for (unsigned int x = 0; x < width; ++x, i += 4) {
      buffer[i] = y * 255 / (height - 1);
      buffer[i + 1] = x * 255 / (width - 1);
      buffer[i + 2] = (width - x - 1) * 255 / (width - 1);
      buffer[i + 3] = 0xff;
    }
  }

  return render::CreateTexture(buffer.get(), width, height);
}

} // namespace

void Init() {}

void CreateGLObjects() {
  render::GLHandle vertex_shader(
      render::CompileShader(kVertexShaderSource, GL_VERTEX_SHADER));
  render::GLHandle fragment_shader(
      render::CompileShader(kFragmentShaderSource, GL_FRAGMENT_SHADER));
  shader_ = render::LinkProgram(vertex_shader, fragment_shader);

  texture_ = CreateGradientTexture(256, 256);

  quad_buffer_ = render::CreateBuffer();

  glBindBuffer(GL_ARRAY_BUFFER, *quad_buffer_);

  GLint attr_pos = glGetAttribLocation(*shader_, "attr_VertexPosition");
  glEnableVertexAttribArray(attr_pos);
  glVertexAttribPointer(attr_pos, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

  GLint attr_texcoord = glGetAttribLocation(*shader_, "attr_TextureCoord");
  glEnableVertexAttribArray(attr_texcoord);
  glVertexAttribPointer(attr_texcoord, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        reinterpret_cast<void*>(2 * sizeof(float)));
}

void RenderFrame(int width, int height) {
  glViewport(0, 0, width, height);

  glUniform2f(glGetUniformLocation(*shader_, "uniform_RcpWindowSize"),
              1.0f / width, 1.0f / height);

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  render::VertexXYUVVector vertices;
  vertices.add_quad(0.0f, 0.0f, 100.0f, 100.0f);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]),
               &vertices[0], GL_STREAM_DRAW);

  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

}  // namespace app
