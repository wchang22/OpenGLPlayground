#include "framebuffer.h"
#include "util/exception.h"
#include "util/data.h"

FrameBuffer::FrameBuffer(int width, int height,
                         int buffer_num_bits, GLenum buffer_type, bool stencil)
  : shader("../../shaders/fb_vertex.glsl", "../../shaders/fb_fragment.glsl")
{
  int color_buffer_format;
  unsigned int rb_storage_type = stencil ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT;
  unsigned int rb_attachment_type = stencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;

  if (buffer_type == GL_UNSIGNED_BYTE) {
    switch (buffer_num_bits) {
      case 8:
        color_buffer_format = GL_RGBA;
        break;
      case 16:
        color_buffer_format = GL_RGBA16;
        break;
      default:
        throw FrameBufferException("Invalid framebuffer size (bits) for byte type: " +
                                   std::to_string(buffer_num_bits));
    }
  } else if (buffer_type == GL_FLOAT) {
    switch (buffer_num_bits) {
      case 16:
        color_buffer_format = GL_RGBA16F;
        break;
      case 32:
        color_buffer_format = GL_RGBA32F;
        break;
      default:
        throw FrameBufferException("Invalid framebuffer size (bits) for float type: " +
                                   std::to_string(buffer_num_bits));
    }
  } else {
    throw FrameBufferException("Invalid framebuffer type");
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenFramebuffers(1, &FBO);
  glGenRenderbuffers(1, &RBO);
  glGenTextures(1, &color_texture);

  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  glBindTexture(GL_TEXTURE_2D, color_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, color_buffer_format, width, height, 0, GL_RGBA, buffer_type, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);

  glBindRenderbuffer(GL_RENDERBUFFER, RBO);
  glRenderbufferStorage(GL_RENDERBUFFER, rb_storage_type, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, rb_attachment_type, GL_RENDERBUFFER, RBO);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw FrameBufferException("Framebuffer not complete");
  }

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof (quadVertices), quadVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof (float), reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof (float),
                        reinterpret_cast<void*>(2 * sizeof (float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteFramebuffers(1, &FBO);
  glDeleteRenderbuffers(1, &RBO);
  glDeleteTextures(1, &color_texture);
}

void FrameBuffer::bind_framebuffer() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
}

void FrameBuffer::draw_scene() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  shader.use_shader_program();
  glDisable(GL_DEPTH_TEST);

  glBindVertexArray(VAO);
  glActiveTexture(GL_TEXTURE0);
  glUniform1i(shader.get_uniform_location("screen_texture"), 0);
  glBindTexture(GL_TEXTURE_2D, color_texture);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}