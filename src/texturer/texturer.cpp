#include "yaml-cpp/node/node.h"
#include "yaml-cpp/parser.h"
#include "yaml-cpp/yaml.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <fstream>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <string>

inline std::string generate_uuid()
{
  static boost::uuids::random_generator gen;
  boost::uuids::uuid id = gen();
  return boost::uuids::to_string(id);
}

glm::dvec3 roundToDecimals(const glm::dvec3& v, int decimals = 6)
{
  glm::dvec3 result = v;
  double factor     = std::pow(10.0, decimals);
  for (int i = 0; i < 3; ++i)
    result[i] = std::round(result[i] * factor) / factor;
  return result;
}

void write_quad_file(std::string filename, std::string path,
                     const bool forceUpdate = false)
{
  std::string fp       = path + '/' + filename;
  YAML::Node ressource = YAML::LoadFile(fp + ".yaml");
  if (!ressource["uuid"]) ressource["uuid"] = generate_uuid();
  std::ofstream fout(fp);
  fout << ressource;

  std::string line, obj;
  std::ifstream quadFileIn(fp + ".quads");
  if (!quadFileIn) goto writeFile;

  std::getline(quadFileIn, line);
  if (forceUpdate || line != ressource["uuid"].as<std::string>())
    goto writeFile;
  else
    return;

writeFile:

  std::ofstream quadFileOut(fp + ".quads", std::ios::trunc);
  int columns =
      ressource["im_width"].as<int>() / ressource["sprite_width"].as<int>();
  int lines =
      ressource["im_height"].as<int>() / ressource["sprite_height"].as<int>();

  quadFileOut << ressource["uuid"] << '\n';

  for (auto it = ressource["faces"].begin(); it != ressource["faces"].end();
       it++)
  {
    obj = it->second["depends"].as<std::string>();

    if (!ressource["baseObj"][obj])
      throw std::runtime_error("Using unexisting base object (" + obj +
                               ") in " + filename);

    for (auto quad = ressource["baseObj"][obj].begin();
         quad != ressource["baseObj"][obj].end(); quad++)
    {
      if (quad->first.as<std::string>() == "tex") continue;
      for (auto vertex = quad->second.begin(); vertex != quad->second.end();
           vertex++)
      {
        glm::dvec3 vec = {vertex->second[0].as<double>(),
                          vertex->second[1].as<double>(),
                          vertex->second[2].as<double>()};

        if (it->second["rotation"])
        {
          double angleX = it->second["rotation"][0].as<double>();
          double angleY = it->second["rotation"][1].as<double>();
          double angleZ = it->second["rotation"][2].as<double>();

          glm::dquat qx =
              glm::angleAxis(glm::radians(angleX), glm::dvec3(1, 0, 0));
          glm::dquat qy =
              glm::angleAxis(glm::radians(angleY), glm::dvec3(0, 1, 0));
          glm::dquat qz =
              glm::angleAxis(glm::radians(angleZ), glm::dvec3(0, 0, 1));
          glm::dquat q = qz * qy * qx; // rotation order
          vec          = q * vec;
        }
        if (it->second["translation"])
          vec += glm::dvec3{it->second["translation"][0].as<double>(),
                            it->second["translation"][1].as<double>(),
                            it->second["translation"][2].as<double>()};
        vec          = roundToDecimals(vec);
        int texIndex = vertex->first.as<int>();
        quadFileOut << vec.x << '\n' << vec.y << '\n' << vec.z << '\n';
        double column = it->second["texPos"].as<int>() % columns;
        double line   = it->second["texPos"].as<int>() / lines;

        double width  = 1.0f / columns;
        double height = 1.0f / lines;

        std::map<int, double> mapX = {
            {0,         column * height},
            {1, column * height + width}
        };
        std::map<int, double> mapY = {
            {0,          line * width},
            {1, line * width + height}
        };

        quadFileOut << mapX[ressource["baseObj"][obj]["tex"]
                                     [std::to_string(texIndex)][0]
                                         .as<int>()]

                    << '\n'
                    << mapY[ressource["baseObj"][obj]["tex"]
                                     [std::to_string(texIndex)][1]
                                         .as<int>()]
                    << '\n';
      }
    }
  }
  return;
}

std::vector<float> readQuadFile(std::string filename, std::string path)
{
  std::vector<float> lines;
  std::ifstream file(path + '/' + filename + ".quads");

  if (!file.is_open())
  {
    std::cout << "owo" << '\n';
    return lines; // return empty vector
  }

  std::string line;
  // Skips UUID
  std::getline(file, line);
  while (std::getline(file, line)) lines.push_back(std::stof(line));

  file.close();
  std::cout << lines.size() << '\n';
  for (const auto& value : lines) std::cout << value << " ";
  std::cout << std::endl;
  return lines;
}
