#include "texturer.hpp"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/parser.h"
#include "yaml-cpp/yaml.h"

#include <fstream>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <string>

TextureFile::TextureFile(std::string _fname) : fname(_fname) {}
TextureFile::TextureFile(std::string _fname, std::string _assetsPath)
    : assetsPath(_assetsPath), fname(_fname)
{
}

glm::dvec3 roundToDecimals(const glm::dvec3& v, int decimals = 6)
{
  glm::dvec3 result = v;
  double factor     = std::pow(10.0, decimals);
  for (int i = 0; i < 3; ++i)
    result[i] = std::round(result[i] * factor) / factor;
  return result;
}

void TextureFile::writeQuadFile() const
{
  const std::string fp       = this->assetsPath + '/' + this->fname;
  const YAML::Node ressource = YAML::LoadFile(fp + ".yaml");
  YAML::Node refObj_res;

  std::string refObj;
  int lines, columns, texIndex;
  double width, height, angleX, angleY, angleZ, column, line;
  this->getMetadata(ressource, lines, columns, width, height);

  std::ofstream quadFileOut(fp + ".quads", std::ios::trunc);
  const YAML::Node& faces_res   = get_res(ressource, FACES);
  const YAML::Node& baseobj_res = get_res(ressource, BASE_OBJ);
  YAML::Node rotation_res, translation_res, tex_res, tex_vertex_res;
  glm::dquat qx, qy, qz;
  glm::dvec3 vec;
  std::map<int, double> mapX, mapY;
  for (auto face = faces_res.begin(); face != faces_res.end(); face++)
  {
    refObj     = sget_res(face->second, DEPENDS);
    refObj_res = get_res(baseobj_res, refObj);

    for (auto quad = refObj_res.begin(); quad != refObj_res.end(); quad++)
    {
      if (quad->first.as<std::string>() == TEX) continue;
      for (auto vertex = quad->second.begin(); vertex != quad->second.end();
           vertex++)
      {
        vec = {vertex->second[0].as<double>(), vertex->second[1].as<double>(),
               vertex->second[2].as<double>()};

        if (face->second[ROT])
        {
          rotation_res = face->second[ROT];
          angleX       = rotation_res[0].as<double>();
          angleY       = rotation_res[1].as<double>();
          angleZ       = rotation_res[2].as<double>();

          qx  = glm::angleAxis(glm::radians(angleX), glm::dvec3(1, 0, 0));
          qy  = glm::angleAxis(glm::radians(angleY), glm::dvec3(0, 1, 0));
          qz  = glm::angleAxis(glm::radians(angleZ), glm::dvec3(0, 0, 1));
          vec = qx * qy * qz * vec;
        }
        if (face->second[TRANS])
        {
          translation_res = face->second[TRANS];
          vec += glm::dvec3{translation_res[0].as<double>(),
                            translation_res[1].as<double>(),
                            translation_res[2].as<double>()};
        }
        vec      = roundToDecimals(vec);
        texIndex = vertex->first.as<int>();
        quadFileOut << vec.x << '\n' << vec.y << '\n' << vec.z << '\n';
        column = iget_res(face->second, TEXPOS) % columns;
        line   = iget_res(face->second, TEXPOS) / lines;

        mapX = {
            {0,         column * height},
            {1, column * height + width}
        };
        mapY = {
            {0,          line * width},
            {1, line * width + height}
        };
        tex_res        = get_res(refObj_res, TEX);
        tex_vertex_res = get_res(tex_res, std::to_string(texIndex));
        quadFileOut << mapX[tex_vertex_res[0].as<int>()] << '\n'
                    << mapY[tex_vertex_res[1].as<int>()] << '\n';
      }
    }
  }
  return;
}

std::vector<float> TextureFile::readQuadFile(void) const
{
  std::vector<float> lines;
  std::ifstream file(this->assetsPath + '/' + this->fname + ".quads");

  if (!file.is_open()) return lines; // return empty vector

  std::string line;
  while (std::getline(file, line)) lines.push_back(std::stof(line));

  file.close();
  return lines;
}

bool TextureFile::isQuadWritten(void) const
{
  std::ifstream f(this->assetsPath + '/' + this->fname + ".quads");
  return f.good();
}
