
#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include "mesh.hpp"

std::vector<Mesh> loadWavefront(std::string const srcFile, bool quiet = false);

Mesh loadTerrainMesh(std::string const srcFile);
