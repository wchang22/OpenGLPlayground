#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <string_view>

#define GENERATE_EXCEPTION_HEADER(name)                                        \
  struct name : public std::runtime_error {                                    \
    name(std::string_view msg);                                                \
  };                                                                           \

#define GENERATE_EXCEPTION_IMPL(name)                                          \
  name::name(std::string_view msg) : std::runtime_error(msg.data()) {}         \

GENERATE_EXCEPTION_HEADER(ShaderException)
GENERATE_EXCEPTION_HEADER(WindowException)
GENERATE_EXCEPTION_HEADER(DisplayException)
GENERATE_EXCEPTION_HEADER(TextureException)
GENERATE_EXCEPTION_HEADER(MeshException)
GENERATE_EXCEPTION_HEADER(ModelException)
GENERATE_EXCEPTION_HEADER(ShadowException)
GENERATE_EXCEPTION_HEADER(FrameBufferException)
GENERATE_EXCEPTION_HEADER(LoggingException)

#endif // EXCEPTION_H
