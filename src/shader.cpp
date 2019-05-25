#include "shader.h"
#include "exception.h"

#include <fstream>
#include <iostream>

#include <glad/glad.h>

Shader::Shader(const char* path_vertex, const char* path_fragment, const char* path_geometry) {
  std::string vertex_source = read_source(path_vertex);
  std::string fragment_source = read_source(path_fragment);
  const char* vertex_source_cstr = vertex_source.c_str();
  const char* fragment_source_cstr = fragment_source.c_str();

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_source_cstr, nullptr);
  glCompileShader(vertex_shader);

  if (!check_shader_errors(vertex_shader)) {
    throw Exception::ShaderException(("Failed to compile " + std::string(path_vertex) +
                                     ", check above log").c_str());
  }

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_source_cstr, nullptr);
  glCompileShader(fragment_shader);

  if (!check_shader_errors(fragment_shader)) {
    throw Exception::ShaderException(("Failed to compile " + std::string(path_fragment) +
                                      ", check above log").c_str());
  }

  shader_program = glCreateProgram();

  if (path_geometry) {
    std::string geometry_source = read_source(path_geometry);
    const char* geometry_source_cstr = geometry_source.c_str();

    geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry_shader, 1, &geometry_source_cstr, nullptr);
    glCompileShader(geometry_shader);

    if (!check_shader_errors(geometry_shader)) {
      throw Exception::ShaderException(("Failed to compile " + std::string(path_geometry) +
                                        ", check above log").c_str());
    }

    glAttachShader(shader_program, geometry_shader);
  }

  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  if (!check_program_errors(shader_program)) {
    throw Exception::ShaderException("Failed to link shaders, check above log");
  }
}

Shader::~Shader() {
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  glDeleteShader(geometry_shader);
  glDeleteProgram(shader_program);
}

void Shader::use_shader_program() const {
  glUseProgram(shader_program);
}

int Shader::get_uniform_location(const char* uniform) const {
  return glGetUniformLocation(shader_program, uniform);
}

std::string Shader::read_source(const char* path) {
  std::ifstream file(path);
  std::string source;

  if (!file.is_open()) {
    throw Exception::ShaderException((std::string("Cannot open file ") +
                                      std::string(path)).c_str());
  }

  std::string line;

  while (std::getline(file, line)) {
    source.append(line + "\n");
  }

  file.close();

  return source;
}

bool Shader::check_shader_errors(unsigned int shader) {
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (success) {
    return true;
  }

  char log[512];
  glGetShaderInfoLog(shader, 512, nullptr, log);
  std::cerr << log << std::endl;

  return false;
}

bool Shader::check_program_errors(unsigned int program){
  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (success) {
    return true;
  }

  char log[512];
  glGetProgramInfoLog(program, 512, nullptr, log);
  std::cerr << log << std::endl;

  return false;
}
