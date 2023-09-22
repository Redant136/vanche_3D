#ifndef VANCHE_VMODELRENDERER
#define VANCHE_VMODELRENDERER
#include <vanche_utils.h>
#include "../loader/model.hpp"
#include <glm/glm.hpp>

#define MAX_LIGHT_SOURCES 8

struct LightSource_t
{
  glm::vec3 pos;
  glm::vec3 color;
  float intensity;
};
struct Camera_t
{
  glm::vec3 pos;
  glm::vec4 rot;
  float zoom;
};
struct Shader_t
{
  uint ID;
};
extern struct WORLD_t
{
  glm::vec3 UP;
  glm::vec3 FRONT;
  glm::vec3 RIGHT;
  LightSource_t lights[MAX_LIGHT_SOURCES];
  Shader_t shader;
  Camera_t camera;
} WORLD;
struct VModel_t
{
  std::string path;
  gltf::glTFModel model;
  glm::vec3 pos;
  glm::mat4*nodeTransforms;
  uint *VAO;
  uint *VBO;
  uint *gltfImageTextureIndex;
};

void shaderSetBool(Shader_t &shader, const std::string &name, bool value);
void shaderSetInt(Shader_t &shader, const std::string &name, int value);
void shaderSetFloat(Shader_t &shader, const std::string &name, float value);
void shaderSetVec2(Shader_t &shader, const std::string &name, const glm::vec2 &value);
void shaderSetVec2(Shader_t &shader, const std::string &name, float x, float y);
void shaderSetVec3(Shader_t &shader, const std::string &name, const glm::vec3 &value);
void shaderSetVec3(Shader_t &shader, const std::string &name, float x, float y, float z);
void shaderSetVec4(Shader_t &shader, const std::string &name, const glm::vec4 &value);
void shaderSetVec4(Shader_t &shader, const std::string &name, float x, float y, float z, float w);
void shaderSetMat2(Shader_t &shader, const std::string &name, const glm::mat2 &mat);
void shaderSetMat3(Shader_t &shader, const std::string &name, const glm::mat3 &mat);
void shaderSetMat4(Shader_t &shader, const std::string &name, const glm::mat4 &mat);
void shaderSetBoolArr(Shader_t &shader, const std::string &name, uint size, bool *value);
void shaderSetIntArr(Shader_t &shader, const std::string &name, uint size, int *value);
void shaderSetFloatArr(Shader_t &shader, const std::string &name, uint size, float *value);
void shaderSetVec2Arr(Shader_t &shader, const std::string &name, uint size, const float *value);
void shaderSetVec3Arr(Shader_t &shader, const std::string &name, uint size, const float *value);
void shaderSetVec4Arr(Shader_t &shader, const std::string &name, uint size, const float *value);
void shaderSetMat2Arr(Shader_t &shader, const std::string &name, uint size, const float *mat);
void shaderSetMat3Arr(Shader_t &shader, const std::string &name, uint size, const float *mat);
void shaderSetMat4Arr(Shader_t &shader, const std::string &name, uint size, const float *mat);

void initVModel(VModel_t*model);
int WORLDExecute(const gltf::glTFModel model);
int renderVModel(const VModel_t &model);

#endif