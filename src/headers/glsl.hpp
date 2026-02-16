#ifndef GLSL_HPP
#define GLSL_HPP

static const auto vertexShaderSource = R"(
#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in mat4 instanceMatrix;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

static const auto fragmentShaderSource = R"(
#version 410 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D tex;

void main()
{
    FragColor = texture(tex, TexCoord);
}
)";

#endif
