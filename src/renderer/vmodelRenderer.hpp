#ifndef VANCHE_VMODELRENDERER
#define VANCHE_VMODELRENDERER
#include <vanche_utils.h>
#include "../loader/model.hpp"
#include <glm/glm.hpp>

#define MAX_JOINT_MATRIX 128
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
  bool updated=true;
  glm::mat4 viewMatrix, projectionMatrix;
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
  Camera_t camera;
  struct
  {
    Shader_t defaultShader;
    Shader_t skeletonShader;
    Shader_t mtoon;
  } shaders;
} WORLD;
struct VModel_t
{
  std::string path;
  gltf::glTFModel model;
  bool updatedPos;
  glm::vec3 pos;
  uint*renderQueue;
  glm::mat4 *nodeTransforms;
  uint **VAO;
  uint *VBO;
  uint *gltfImageTextureIndex;
  uint sampler_obj;
  bool **updatedMorphWeight;
  glm::vec3 ***morphs;
  uint**morphsVBO;
  uint UBO;
};

void shaderSetBool(const Shader_t &shader, const std::string &name, bool value);
void shaderSetInt(const Shader_t &shader, const std::string &name, int value);
void shaderSetFloat(const Shader_t &shader, const std::string &name, float value);
void shaderSetVec2(const Shader_t &shader, const std::string &name, const glm::vec2 &value);
void shaderSetVec2(const Shader_t &shader, const std::string &name, float x, float y);
void shaderSetVec3(const Shader_t &shader, const std::string &name, const glm::vec3 &value);
void shaderSetVec3(const Shader_t &shader, const std::string &name, float x, float y, float z);
void shaderSetVec4(const Shader_t &shader, const std::string &name, const glm::vec4 &value);
void shaderSetVec4(const Shader_t &shader, const std::string &name, float x, float y, float z, float w);
void shaderSetMat2(const Shader_t &shader, const std::string &name, const glm::mat2 &mat);
void shaderSetMat3(const Shader_t &shader, const std::string &name, const glm::mat3 &mat);
void shaderSetMat4(const Shader_t &shader, const std::string &name, const glm::mat4 &mat);
void shaderSetBoolArr(const Shader_t &shader, const std::string &name, uint size, bool *value);
void shaderSetIntArr(const Shader_t &shader, const std::string &name, uint size, int *value);
void shaderSetFloatArr(const Shader_t &shader, const std::string &name, uint size, float *value);
void shaderSetVec2Arr(const Shader_t &shader, const std::string &name, uint size, const float *value);
void shaderSetVec3Arr(const Shader_t &shader, const std::string &name, uint size, const float *value);
void shaderSetVec4Arr(const Shader_t &shader, const std::string &name, uint size, const float *value);
void shaderSetMat2Arr(const Shader_t &shader, const std::string &name, uint size, const float *mat);
void shaderSetMat3Arr(const Shader_t &shader, const std::string &name, uint size, const float *mat);
void shaderSetMat4Arr(const Shader_t &shader, const std::string &name, uint size, const float *mat);

void initVModel(VModel_t *vmodel);
int WORLDExecute(const gltf::glTFModel model);
int renderVModel(VModel_t &vmodel);
void freeVModel(VModel_t*vmodel);

#endif