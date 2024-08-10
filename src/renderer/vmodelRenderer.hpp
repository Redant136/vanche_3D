#ifndef VANCHE_VMODELRENDERER
#define VANCHE_VMODELRENDERER
#include <vanche_utils.h>
#include "../loader/vanche_model.hpp"
#include <glm/glm.hpp>

#define MAX_JOINT_MATRIX 256
#define MAX_LIGHT_SOURCES 8
#define MAX_NODES 512

struct LightSource_t
{
  glm::vec3 pos;
  float intensity;
  glm::vec3 color;
  float _padding;
};
struct Camera_t
{
  glm::vec3 pos;
  glm::vec4 rot;
  float zoom;
  bool updated = true;
  glm::mat4 viewMatrix, projectionMatrix;
};
extern struct WORLD_t
{
  glm::vec3 UP;
  glm::vec3 FRONT;
  glm::vec3 RIGHT;
  uint lightsUBO;
  LightSource_t lights[MAX_LIGHT_SOURCES];
  Camera_t camera;
  struct
  {
    uint defaultShader;
    uint skeletonShader;
    uint mtoon;
  } shaders;
} WORLD;
struct VModel_t
{
  std::string path;
  gltf::glTFModel model;
  glm::vec3 pos;
  uint **VAO;
  uint *VBO;
  uint *gltfImageTextureIndex;
  uint sampler_obj;
  bool *updatedMorphWeight;
  glm::vec3 ***morphs;
  uint *morphsVBO;
  glm::vec4 *materialColorTransforms;
  glm::vec2 *materialTextureTransform;
  uint UBO;
  uint nodesUBO;
  uchar**accessorBuffers;
};

void WORLDInit();
void initVModel(VModel_t *vmodel);
int updateVModel(VModel_t *vmodel);
int renderVModel(VModel_t vmodel);
void freeVModel(VModel_t *vmodel);

void vmodelSetMorphWeight(VModel_t *vmodel, uint mesh, uint weight, float weightVal);
void vmodelVRMSetMorphWeight(VModel_t *vmodel, uint mesh, std::string target, float weight);
void vmodelSetVRMExpressions(VModel_t *vmodel, float *values);
int vmodelGetVRMNode(VModel_t*vmodel, std::string name);

#endif