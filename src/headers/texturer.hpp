#ifndef TEXTURER_HPP
#define TEXTURER_HPP

#include <iostream>
#include <vector>

void write_quad_file(std::string filename, std::string path,
                     const bool forceUpdate = false);

std::vector<float> readQuadFile(std::string filename, std::string path);
#endif
