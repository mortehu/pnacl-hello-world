#include <err.h>

#include <SDL/SDL.h>

#include "app.h"
#include "gl.h"

namespace {

int width = 640, height = 480;

void ProcessEvents() {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_VIDEORESIZE:
        width = event.resize.w;
        height = event.resize.h;
        SDL_SetVideoMode(width, height, 0, SDL_OPENGL | SDL_RESIZABLE);
        break;

      case SDL_QUIT:
        exit(EXIT_SUCCESS);
        break;
    }
  }
}

}  // namespace

int main(int argc, char** argv) {
  if (0 != SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE))
    errx(EXIT_FAILURE, "SDL_Init failed: %s", SDL_GetError());

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

  if (!SDL_SetVideoMode(width, height, 0, SDL_OPENGL | SDL_RESIZABLE))
    errx(EXIT_FAILURE, "SDL_SetVideoMode failed: %s", SDL_GetError());

#if USING_GLEW
  glewInit();
#endif

  app::Init();
  app::CreateGLObjects();

  for (;;) {
    ProcessEvents();
    app::RenderFrame(width, height);
    SDL_GL_SwapBuffers();
  }
}
