// Based on
// https://github.com/gruberchris/hello_opengl_and_sdl3/blob/main/main.cpp
// and
// https://learnopengl.com/code_viewer_gh.php?code=src/4.advanced_opengl/10.3.asteroids_instanced/asteroids_instanced.cpp
#define STB_IMAGE_IMPLEMENTATION
#include "arcball.hpp"
#include "camera.hpp"
#include "ex_cube.hpp"
#include "opengl.hpp"
#include "shaders.hpp"
#include "stb_image.h"

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
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  const GLuint shaderProgram = createShaderProgram();

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // Set wrapping / filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Load image
  int width, height, nrChannels;
  unsigned char* data =
      stbi_load("src/brick.jpeg", &width, &height, &nrChannels, 0);
  if (data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 nrChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  stbi_image_free(data);

  glUseProgram(shaderProgram);
  glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

  GLuint VAO, VBO;
  setupCubeData(VAO, VBO);

  Camera camera;
  bool running    = true;
  Uint64 lastTime = SDL_GetTicks();

  int windowWidth  = WINDOW_WIDTH;
  int windowHeight = WINDOW_HEIGHT;

  Arcball arcball = Arcball(windowWidth, windowHeight);

  const GLint viewLoc       = glGetUniformLocation(shaderProgram, "view");
  const GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

  // Cube model
  std::vector<glm::mat4> matrices;
  for (int j = 0; j < 10; j++)
    for (int i = 0; i < 2; i++)
      for (int _ = 0; _ < 5; _++)
      {
        auto model = glm::mat4(1.0f);
        model      = glm::translate(
            model, glm::vec3{(i == 0 ? 1 : -1) * 2 * _, -8, -2 * j});
        matrices.push_back(model);
      }

  unsigned int buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4),
               matrices.data(), GL_STATIC_DRAW);

  glBindVertexArray(VAO);
  // set attribute pointers for matrix (4 times vec4)
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void*)(sizeof(glm::vec4)));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void*)(2 * sizeof(glm::vec4)));
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void*)(3 * sizeof(glm::vec4)));

  glVertexAttribDivisor(2, 1);
  glVertexAttribDivisor(3, 1);
  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);

  glBindVertexArray(0);

  // MAIN LOOP

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

    // View matrix (camera)
    auto view = glm::mat4(1.0f);
    view      = glm::translate(view, glm::vec3(0.0f, 0.0f, -camera.distance));
    view *= arcball.rotate();

    // Projection matrix
    const float aspect =
        static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, matrices.size());

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
