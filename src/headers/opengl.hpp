#ifndef OPENGL_HPP_CUSTOM
#define OPENGL_HPP_CUSTOM

#include "yaml-cpp/node/node.h"
#include "yaml-cpp/parser.h"
#include "yaml-cpp/yaml.h"
#include <SDL3/SDL.h>
#include <iostream>

void set_opengl_attribute(void)
{

  YAML::Node config = YAML::LoadFile("src/config.yaml");

  const int major_version = config["MAJOR_VERSION"].as<int>();
  const int minor_version = config["MINOR_VERSION"].as<int>();
  const int doublebuffer  = config["DOUBLEBUFFER"].as<int>();
  const int depthsize     = config["DEPTH_SIZE"].as<int>();

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major_version);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor_version);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, doublebuffer);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depthsize);
}

#endif
