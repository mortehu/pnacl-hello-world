#include <GLES2/gl2.h>

#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/graphics_3d.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/lib/gl/gles2/gl2ext_ppapi.h"

#include "app.h"

namespace {

class Instance : public pp::Instance {
 public:
  explicit Instance(PP_Instance instance) : pp::Instance(instance) {
    app::Init();
  }

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

    app::CreateGLObjects();

    return true;
  }

  void FrameCallback() {
    app::RenderFrame(width_, height_);
    context_.SwapBuffers(pp::CompletionCallback([](void* this_, int32_t) {
                                                  reinterpret_cast<Instance*>(
                                                      this_)->FrameCallback();
                                                },
                                                this));
  }

  pp::Graphics3D context_;
  int32_t width_;
  int32_t height_;
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
