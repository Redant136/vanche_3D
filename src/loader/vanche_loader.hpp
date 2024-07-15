#ifndef VANCHE_LOADER
#define VANCHE_LOADER
#include <vanche_utils.h>
#include "vanche_model.hpp"

enum class Filetype
{
  unknown = -1,
  glb,
  vrm,
  gltf
};

gltf::glTFModel loadModel(std::string path);
#endif