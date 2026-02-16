#ifndef EX_CUBE_HPP
#define EX_CUBE_HPP

#include <GL/glew.h>

void setupCubeData(GLuint& VAO, GLuint& VBO)
{

  constexpr float vertices[] = {
      // Positions // Colors (gradients)
      // Front face - Red to Yellow
      -1,
      -1,
      1,
      0.5,
      1,

      1,
      -1,
      1,
      1,
      1,

      1,
      1,
      1,
      1,
      0,

      1,
      1,
      1,
      1,
      0,

      -1,
      1,
      1,
      0.5,
      0,

      -1,
      -1,
      1,
      0.5,
      1,

      // // Back face - Blue to Cyan
      -1,
      -1,
      -1,
      0.5,
      0,

      -1,
      1,
      -1,
      1,
      0,

      1,
      1,
      -1,
      1,
      1,

      1,
      1,
      -1,
      1,
      1,

      1,
      -1,
      -1,
      0.5,
      1,

      -1,
      -1,
      -1,
      0.5,
      0,

      // Top face - Green to Yellow
      -1,
      1,
      -1,
      0,
      0,

      -1,
      1,
      1,
      0.5,
      0,

      1,
      1,
      1,
      0.5,
      1,

      1,
      1,
      1,
      0.5,
      1,

      1,
      1,
      -1,
      0,
      1,

      -1,
      1,
      -1,
      0,
      0,

      // Bottom face - Magenta to Purple
      -1,
      -1,
      -1,
      0.5,
      0,

      1,
      -1,
      -1,
      1,
      0,

      1,
      -1,
      1,
      1,
      1,

      1,
      -1,
      1,
      1,
      1,

      -1,
      -1,
      1,
      0.5,
      1,

      -1,
      -1,
      -1,
      0.5,
      0,

      // Right face - Orange to Pink
      1,
      -1,
      -1,
      0.5,
      0,

      1,
      1,
      -1,
      1,
      0,

      1,
      1,
      1,
      1,
      1,

      1,
      1,
      1,
      1,
      1,

      1,
      -1,
      1,
      0.5,
      1,

      1,
      -1,
      -1,
      0.5,
      0,

      // Left face - Teal to Lime
      -1,
      -1,
      -1,
      0.5,
      0,

      -1,
      -1,
      1,
      1,
      0,

      -1,
      1,
      1,
      1,
      1,

      -1,
      1,
      1,
      1,
      1,

      -1,
      1,
      -1,
      0.5,
      1,

      -1,
      -1,
      -1,
      0.5,
      0,
  };

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

#endif
