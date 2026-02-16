// Based on
// https://github.com/gruberchris/hello_opengl_and_sdl3/blob/main/main.cpp

#include "arcball.hpp"
#include "camera.hpp"
#include "ex_cube.hpp"
#include "opengl.hpp"
#include "shaders.hpp"

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

constexpr int WINDOW_WIDTH  = 1024;
constexpr int WINDOW_HEIGHT = 768;

int main(int argc, char* argv[])
{
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
    return 1;
  }

  set_opengl_attribute();

  SDL_Window* window =
      SDL_CreateWindow("OpenGL 3D Cube - SDL3", WINDOW_WIDTH, WINDOW_HEIGHT,
                       SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

  if (!window)
  {
    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  SDL_GLContext glContext = SDL_GL_CreateContext(window);
  if (!glContext)
  {
    std::cerr << "SDL_GL_CreateContext failed: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_GL_SetSwapInterval(1);

  if (glewInit() != GLEW_OK)
  {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    return 1;
  }

  print_opengl_infos();

  // Setup OpenGL
  glEnable(GL_DEPTH_TEST);

  const GLuint shaderProgram = createShaderProgram();
  GLuint VAO, VBO;
  setupCubeData(VAO, VBO);

  Camera camera;
  bool running    = true;
  Uint64 lastTime = SDL_GetTicks();

  int windowWidth  = WINDOW_WIDTH;
  int windowHeight = WINDOW_HEIGHT;

  Arcball arcball = Arcball(windowWidth, windowHeight);

  while (running)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_EVENT_QUIT: running = false; break;
      case SDL_EVENT_KEY_DOWN:
        switch (event.key.key)
        {
        case SDLK_ESCAPE: running = false; break;
        case SDLK_EQUALS:
        case SDLK_PLUS:
        case SDLK_KP_PLUS: camera.zoomIn(); break;
        case SDLK_MINUS:
        case SDLK_KP_MINUS: camera.zoomOut(); break;
        default:;
        }
        break;
      case SDL_EVENT_WINDOW_RESIZED:
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);
        arcball.updateDims(windowWidth, windowHeight);
        break;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        arcball.initRotation(event.button.x, event.button.y);
        break;
      case SDL_EVENT_MOUSE_BUTTON_UP: arcball.endRotation(); break;
      case SDL_EVENT_MOUSE_MOTION:
        if (event.motion.state & SDL_BUTTON_LMASK)
          arcball.computeRotation(event.motion.x, event.motion.y);
        break;
      default:;
      }
    }

    const Uint64 currentTime = SDL_GetTicks();
    const float deltaTime =
        static_cast<float>(currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    camera.update(deltaTime);

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    // Rotation
    auto model = glm::mat4(1.0f);
    model *= arcball.rotate();

    // View matrix (camera)
    auto view = glm::mat4(1.0f);
    view      = glm::translate(view, glm::vec3(0.0f, 0.0f, -camera.distance));

    // Projection matrix
    const float aspect =
        static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    const GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    const GLint viewLoc  = glGetUniformLocation(shaderProgram, "view");
    const GLint projectionLoc =
        glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    SDL_GL_SwapWindow(window);
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  SDL_GL_DestroyContext(glContext);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
