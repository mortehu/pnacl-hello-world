#ifndef VERTEX_H_
#define VERTEX_H_ 1

#include <vector>

namespace render {

struct VertexXYUV {
  VertexXYUV(float x, float y, float u, float v) : x(x), y(y), u(u), v(v) {}

  float x, y;
  float u, v;
};

class VertexXYUVVector : public std::vector<VertexXYUV> {
 public:
  void add_quad(float x, float y, float width, float height) {
    add_quad_uv(x, y, width, height, 0.0f, 0.0f, 1.0f, 1.0f);
  }

  void add_quad_uv(float x, float y, float width, float height, float u0,
                   float v0, float u1, float v1) {
    emplace_back(x, y, u0, u1);
    emplace_back(x, y + height, u0, u0);
    emplace_back(x + width, y + height, u1, u0);

    emplace_back(x, y, u0, u1);
    emplace_back(x + width, y + height, u1, u0);
    emplace_back(x + width, y, u1, u1);
  }
};

}  // namespace render

#endif /* !VERTEX_H_ */
