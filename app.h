// Defines the interface between system code and application code.

#ifndef APP_H_
#define APP_H_

namespace app {

void Init();

// Creates OpenGL objects such as programs, textures and vertex buffer objects.
//
// Some platforms may lose their OpenGL context entirely at any point in time,
// which is why this function is separate from Init().
void CreateGLObjects();

// Renders one animation frame.
void RenderFrame(int width, int height);

}  // namespace app

#endif  // !APP_H_
