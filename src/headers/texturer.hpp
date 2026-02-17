#ifndef TEXTURER_HPP
#define TEXTURER_HPP

#include <iostream>
#include <vector>

#include "yaml-cpp/yaml.h"
void write_quad_file(std::string filename, std::string path,
                     const bool forceUpdate = false);

std::vector<float> readQuadFile(std::string filename, std::string path);

class TextureFile
{
public:
  TextureFile(std::string _name);
  TextureFile(std::string _name, std::string _assetsPath);

  void writeQuadFile(void) const;
  std::vector<float> readQuadFile(void) const;

  bool isQuadWritten(void) const;

private:
  std::string assetsPath;
  std::string fname;

  static constexpr std::string FACES = "faces";

  static constexpr std::string IMAGE_HEIGHT  = "im_height";
  static constexpr std::string IMAGE_WIDTH   = "im_width";
  static constexpr std::string SPRITE_HEIGHT = "sprite_height";
  static constexpr std::string SPRITE_WIDTH  = "sprite_height";

  static constexpr std::string BASE_OBJ = "baseObj";

  static constexpr std::string DEPENDS = "depends";

  static constexpr std::string ROT   = "rotation";
  static constexpr std::string TRANS = "translation";

  static constexpr std::string TEX    = "tex";
  static constexpr std::string TEXPOS = "texPos";

  static inline int iget_res(const YAML::Node& ressource, const std::string key)
  {
    return get_res(ressource, key).as<int>();
  }
  static inline std::string sget_res(const YAML::Node& ressource,
                                     const std::string key)
  {
    return get_res(ressource, key).as<std::string>();
  }
  static inline YAML::Node get_res(const YAML::Node& ressource,
                                   const std::string key)
  {
    if (!ressource[key]) throw std::runtime_error("Key error " + key);
    return ressource[key];
  }

  static inline void getMetadata(const YAML::Node& ressource, int& lines,
                                 int& columns, double& width, double& height)
  {
    lines =
        iget_res(ressource, IMAGE_HEIGHT) / iget_res(ressource, SPRITE_HEIGHT);
    columns =
        iget_res(ressource, IMAGE_WIDTH) / iget_res(ressource, SPRITE_WIDTH);
    width  = 1.0f / columns;
    height = 1.0f / lines;
  }
};

#endif
