#include <GLES2/gl2.h>

#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/graphics_3d.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/lib/gl/gles2/gl2ext_ppapi.h"

#include "gl.h"
#include "vertex.h"

namespace {

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

class Instance : public pp::Instance {
 public:
  explicit Instance(PP_Instance instance) : pp::Instance(instance) {}

  void DidChangeView(const pp::View& view) override {
    int32_t new_width = view.GetRect().width();
    int32_t new_height = view.GetRect().height();

    if (context_.is_null()) {
      if (!InitGL(new_width, new_height)) {
        // failed.
        return;
      }
      FrameCallback();
    } else {
      int32_t result = context_.ResizeBuffers(new_width, new_height);
      if (result < 0) {
        fprintf(stderr, "Unable to resize buffers to %d x %d!\n", new_width,
                new_height);
        return;
      }
    }

    width_ = new_width;
    height_ = new_height;
    glViewport(0, 0, width_, height_);

    glUniform2f(glGetUniformLocation(*shader_, "uniform_RcpWindowSize"),
                1.0f / width_, 1.0f / height_);
  }

  void HandleMessage(const pp::Var& var_message) override {}

 private:
  bool InitGL(int32_t new_width, int32_t new_height) {
    if (!glInitializePPAPI(pp::Module::Get()->get_browser_interface())) {
      fprintf(stderr, "Unable to initialize GL PPAPI!\n");
      return false;
    }

    const int32_t attrib_list[] = {PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
                                   PP_GRAPHICS3DATTRIB_WIDTH,      new_width,
                                   PP_GRAPHICS3DATTRIB_HEIGHT,     new_height,
                                   PP_GRAPHICS3DATTRIB_NONE};

    context_ = pp::Graphics3D(this, attrib_list);
    if (!BindGraphics(context_)) {
      fprintf(stderr, "Unable to bind 3d context!\n");
      context_ = pp::Graphics3D();
      glSetCurrentContextPPAPI(0);
      return false;
    }

    glSetCurrentContextPPAPI(context_.pp_resource());

    SetupShaders();

    return true;
  }

  void SetupShaders() {
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
    glVertexAttribPointer(attr_pos, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          0);

    GLint attr_texcoord = glGetAttribLocation(*shader_, "attr_TextureCoord");
    glEnableVertexAttribArray(attr_texcoord);
    glVertexAttribPointer(attr_texcoord, 2, GL_FLOAT, GL_FALSE,
                          4 * sizeof(float),
                          reinterpret_cast<void*>(2 * sizeof(float)));
  }

  void FrameCallback() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    render::VertexXYUVVector vertices;
    vertices.add_quad(0.0f, 0.0f, 100.0f, 100.0f);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]),
                 &vertices[0], GL_STREAM_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    context_.SwapBuffers(pp::CompletionCallback([](void* this_, int32_t) {
                                                  reinterpret_cast<Instance*>(
                                                      this_)->FrameCallback();
                                                },
                                                this));
  }

  pp::Graphics3D context_;
  int32_t width_;
  int32_t height_;

  render::GLHandle quad_buffer_;
  render::GLHandle texture_;
  render::GLHandle shader_;
};

class Module : public pp::Module {
 public:
  pp::Instance* CreateInstance(PP_Instance instance) override {
    return new Instance(instance);
  }
};

}  // namespace

namespace pp {

Module* CreateModule() { return new ::Module; }

}  // namespace pp
