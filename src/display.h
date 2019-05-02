#ifndef DISPLAY_H
#define DISPLAY_H

#include <memory>
#include "camera.h"

#include "shader.h"

class Display {
public:
  Display(std::shared_ptr<Camera> camera);
  ~Display();

  void draw() const;

private:
  unsigned int VAO, VBO, EBO;
  static const unsigned int num_textures = 2;
  unsigned int textures[num_textures];

  static void* buffer_offset(int offset);
  void init_buffers();
  void init_textures();
  void init_shaders();

  std::unique_ptr<Shader> shaders;
  std::shared_ptr<Camera> camera;
};

#endif // DISPLAY_H
