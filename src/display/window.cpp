#include "window.h"
#include "util/exception.h"
#include "util/data.h"
#include "util/profiling/profiling.h"

constexpr float MOUSE_SENSITIVITY = 0.05f;

Window::Window()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, NUM_AA_SAMPLES);

  const int width = Window::width();
  const int height = Window::height();
  window = glfwCreateWindow(width, height, "LearnOpenGL", glfwGetPrimaryMonitor(), nullptr);

  if (!window) {
    glfwDestroyWindow(window);
    glfwTerminate();
    throw WindowException("Failed to create GLFW Window");
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    glfwDestroyWindow(window);
    glfwTerminate();
    throw WindowException("Failed to create initialize GLAD");
  }

  glViewport(0, 0, width, height);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  camera = std::make_shared<Camera>(vec3(0.0f, 2.0f, 4.0f),
                                    vec3(0.0f, 0.0f, -1.0f),
                                    vec3(0.0f, 1.0f, 0.0f));

  try {
    display = std::make_unique<Display>(camera);
  } catch (...) {
    glfwDestroyWindow(window);
    std::rethrow_exception(std::current_exception());
  }
}

Window::~Window() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void Window::main_loop() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_FRAMEBUFFER_SRGB);

  try {
    while (!glfwWindowShouldClose(window)) {
      PROFILE_SCOPE("Main Loop")

      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      PROFILE_SECTION_START("Update Camera")
      camera->update_frames();
      PROFILE_SECTION_END()

      PROFILE_SECTION_START("Draw display")
      display->draw();
      PROFILE_SECTION_END()

      PROFILE_SECTION_START("Swap buffers")
      glfwSwapBuffers(window);
      PROFILE_SECTION_END()

      PROFILE_SECTION_START("IO Events")
      glfwPollEvents();
      key_callback();
      mouse_callback();
      PROFILE_SECTION_END()
    }
  } catch (...) {
    glfwDestroyWindow(window);
    std::rethrow_exception(std::current_exception());
  }
}

int Window::width()
{
  return glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
}

int Window::height()
{
  return glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  (void) window;
  glViewport(0, 0, width, height);
}

void Window::key_callback() {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
    cycle_fill_mode();
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera->move(Camera::Direction::FORWARD);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera->move(Camera::Direction::BACKWARD);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera->move(Camera::Direction::LEFT);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera->move(Camera::Direction::RIGHT);
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    camera->move(Camera::Direction::UP);
  }
  if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
    camera->move(Camera::Direction::DOWN);
  }
}

void Window::mouse_callback() {
  static double prev_x, prev_y;
  double x, y;
  glfwGetCursorPos(window, &x, &y);

  if (static bool first_time = true; first_time) {
    prev_x = x;
    prev_y = y;
    first_time = false;
  }

  float delta_x = static_cast<float>(x - prev_x);
  float delta_y = static_cast<float>(prev_y - y);
  prev_x = x;
  prev_y = y;

  delta_x *= MOUSE_SENSITIVITY;
  delta_y *= MOUSE_SENSITIVITY;

  camera->update_direction(delta_x, delta_y);
}

void Window::cycle_fill_mode() {
  switch (static GLenum fill_mode = GL_FILL; fill_mode) {
    case GL_FILL:
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      fill_mode = GL_LINE;
      break;
    case GL_LINE:
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      fill_mode = GL_FILL;
      break;
  }
}
