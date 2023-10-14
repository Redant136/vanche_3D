#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "vmodelRenderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "window.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define vec4ToQua(v) glm::qua<float>(v.x, v.y, v.z, v.w)

#define defaultShaderSource "../src/shader/default/shader.vert", "../src/shader/default/shader.geom", "../src/shader/default/shader.frag"
#define mtoonShaderSource "../src/shader/default/shader.vert", "../src/shader/MToon/MToon.geom", "../src/shader/MToon/MToon.frag"
#define skeletonShaderSource "../src/shader/skeletonRenderer/skeleton.vert", \
                             "../src/shader/skeletonRenderer/skeleton.geom", \
                             "../src/shader/skeletonRenderer/skeleton.frag"

WORLD_t WORLD;
glm::mat4 worldTransform;
glm::mat4 modelTransform;

void shaderSetBool(const Shader_t &shader, const std::string &name, bool value)
{
  glUniform1i(glGetUniformLocation(shader.ID, name.c_str()), (int)value);
}
void shaderSetInt(const Shader_t &shader, const std::string &name, int value)
{
  glUniform1i(glGetUniformLocation(shader.ID, name.c_str()), value);
}
void shaderSetFloat(const Shader_t &shader, const std::string &name, float value)
{
  glUniform1f(glGetUniformLocation(shader.ID, name.c_str()), value);
}
void shaderSetVec2(const Shader_t &shader, const std::string &name, const glm::vec2 &value)
{
  glUniform2fv(glGetUniformLocation(shader.ID, name.c_str()), 1, &value[0]);
}
void shaderSetVec2(const Shader_t &shader, const std::string &name, float x, float y)
{
  glUniform2f(glGetUniformLocation(shader.ID, name.c_str()), x, y);
}
void shaderSetVec3(const Shader_t &shader, const std::string &name, const glm::vec3 &value)
{
  glUniform3fv(glGetUniformLocation(shader.ID, name.c_str()), 1, &value[0]);
}
void shaderSetVec3(const Shader_t &shader, const std::string &name, float x, float y, float z)
{
  glUniform3f(glGetUniformLocation(shader.ID, name.c_str()), x, y, z);
}
void shaderSetVec4(const Shader_t &shader, const std::string &name, const glm::vec4 &value)
{
  glUniform4fv(glGetUniformLocation(shader.ID, name.c_str()), 1, &value[0]);
}
void shaderSetVec4(const Shader_t &shader, const std::string &name, float x, float y, float z, float w)
{
  glUniform4f(glGetUniformLocation(shader.ID, name.c_str()), x, y, z, w);
}
void shaderSetMat2(const Shader_t &shader, const std::string &name, const glm::mat2 &mat)
{
  glUniformMatrix2fv(glGetUniformLocation(shader.ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void shaderSetMat3(const Shader_t &shader, const std::string &name, const glm::mat3 &mat)
{
  glUniformMatrix3fv(glGetUniformLocation(shader.ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void shaderSetMat4(const Shader_t &shader, const std::string &name, const glm::mat4 &mat)
{
  glUniformMatrix4fv(glGetUniformLocation(shader.ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void shaderSetBoolArr(const Shader_t &shader, const std::string &name, uint size, bool *value)
{
  glUniform1iv(glGetUniformLocation(shader.ID, name.c_str()), size, (int *)value);
}
void shaderSetIntArr(const Shader_t &shader, const std::string &name, uint size, int *value)
{
  glUniform1iv(glGetUniformLocation(shader.ID, name.c_str()), size, value);
}
void shaderSetFloatArr(const Shader_t &shader, const std::string &name, uint size, float *value)
{
  glUniform1fv(glGetUniformLocation(shader.ID, name.c_str()), size, value);
}
void shaderSetVec2Arr(const Shader_t &shader, const std::string &name, uint size, const float *value)
{
  glUniform2fv(glGetUniformLocation(shader.ID, name.c_str()), size, value);
}
void shaderSetVec3Arr(const Shader_t &shader, const std::string &name, uint size, const float *value)
{
  glUniform3fv(glGetUniformLocation(shader.ID, name.c_str()), size, value);
}
void shaderSetVec4Arr(const Shader_t &shader, const std::string &name, uint size, const float *value)
{
  glUniform4fv(glGetUniformLocation(shader.ID, name.c_str()), size, value);
}
void shaderSetMat2Arr(const Shader_t &shader, const std::string &name, uint size, const float *mat)
{
  glUniformMatrix2fv(glGetUniformLocation(shader.ID, name.c_str()), size, GL_FALSE, mat);
}
void shaderSetMat3Arr(const Shader_t &shader, const std::string &name, uint size, const float *mat)
{
  glUniformMatrix3fv(glGetUniformLocation(shader.ID, name.c_str()), size, GL_FALSE, mat);
}
void shaderSetMat4Arr(const Shader_t &shader, const std::string &name, uint size, const float *mat)
{
  glUniformMatrix4fv(glGetUniformLocation(shader.ID, name.c_str()), size, GL_FALSE, mat);
}

static Shader_t createShader(std::string vertexPath, std::string geometryPath, std::string fragmentPath)
{
  size_t vLength, gLength, fLength;
  char *vertexCode = 0, *geometryCode = 0, *fragmentCode = 0;
  ch_bufferFile(vertexPath.c_str(), (void **)&vertexCode, &vLength);
  ch_bufferFile(geometryPath.c_str(), (void **)&geometryCode, &gLength);
  ch_bufferFile(fragmentPath.c_str(), (void **)&fragmentCode, &fLength);
  assert(vertexCode && geometryCode && fragmentCode);

  uint vertex, geometry, fragment;
  GLint status;
  GLchar infoLog[1024];
  vertex = glCreateShader(GL_VERTEX_SHADER);
  GLint _vLength = vLength;
  glShaderSource(vertex, 1, &vertexCode, &_vLength);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &status);
  if (!status)
  {
    glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
    fprintf(stderr, "SHADER COMPILATION ERROR\n error of type %d in %s\ninfo log: %s", status, vertexPath.c_str(), infoLog);
    exit(1);
  }

  geometry = glCreateShader(GL_GEOMETRY_SHADER);
  GLint _gLength = gLength;
  glShaderSource(geometry, 1, &geometryCode, &_gLength);
  glCompileShader(geometry);
  glGetShaderiv(geometry, GL_COMPILE_STATUS, &status);
  if (!status)
  {
    glGetShaderInfoLog(geometry, 1024, NULL, infoLog);
    fprintf(stderr, "SHADER COMPILATION ERROR\n error of type %d in %s\ninfo log: %s", status, geometryPath.c_str(), infoLog);
    exit(1);
  }

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  GLint _fLength = fLength;
  glShaderSource(fragment, 1, &fragmentCode, &_fLength);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &status);
  if (!status)
  {
    glGetShaderInfoLog(fragment, 1024, NULL, infoLog);
    fprintf(stderr, "SHADER COMPILATION ERROR\n error of type %d in %s\ninfo log: %s", status, fragmentPath.c_str(), infoLog);
    exit(1);
  }

  uint ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, geometry);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);
  glGetProgramiv(ID, GL_LINK_STATUS, &status);
  if (!status)
  {
    glGetProgramInfoLog(ID, 1024, NULL, infoLog);
    fprintf(stderr, "SHADER LINKING ERROR\n error of type: %d\ninfo log: %s", status, infoLog);
    exit(1);
  }
  glDeleteShader(vertex);
  glDeleteShader(geometry);
  glDeleteShader(fragment);

  Shader_t shader;
  shader.ID = ID;
  free(vertexCode);
  free(geometryCode);
  free(fragmentCode);
  return shader;
}

static void updateVModelNodeTransform(VModel_t *vmodel, uint node, glm::mat4 parentTransform)
{
  glm::mat4 mat = glm::mat4(1), T = glm::mat4(1), R = glm::mat4(1), S = glm::mat4(1);
  S = glm::scale(glm::mat4(1.f), glm::vec3(vmodel->model.nodes[node].scale[0], vmodel->model.nodes[node].scale[1], vmodel->model.nodes[node].scale[2]));
  R = glm::mat4_cast(glm::quat(vmodel->model.nodes[node].rotation[3], vmodel->model.nodes[node].rotation[0], vmodel->model.nodes[node].rotation[1], vmodel->model.nodes[node].rotation[2]));
  T = glm::translate(glm::mat4(1.f), glm::vec3(vmodel->model.nodes[node].translation[0], vmodel->model.nodes[node].translation[1], vmodel->model.nodes[node].translation[2]));
  mat = glm::mat4(
      vmodel->model.nodes[node].matrix[0], vmodel->model.nodes[node].matrix[1], vmodel->model.nodes[node].matrix[2], vmodel->model.nodes[node].matrix[3],
      vmodel->model.nodes[node].matrix[4], vmodel->model.nodes[node].matrix[5], vmodel->model.nodes[node].matrix[6], vmodel->model.nodes[node].matrix[7],
      vmodel->model.nodes[node].matrix[8], vmodel->model.nodes[node].matrix[9], vmodel->model.nodes[node].matrix[10], vmodel->model.nodes[node].matrix[11],
      vmodel->model.nodes[node].matrix[12], vmodel->model.nodes[node].matrix[13], vmodel->model.nodes[node].matrix[14], vmodel->model.nodes[node].matrix[15]);

  vmodel->nodeTransforms[node] = parentTransform * mat * T * R * S;
  for (uint i : vmodel->model.nodes[node].children)
  {
    updateVModelNodeTransform(vmodel, i, vmodel->nodeTransforms[node]);
  }
}

static void mergeSortRenderQueue(VModel_t *vmodel, uint *queue, uint length)
{
  if (length < 2)
    return;
  mergeSortRenderQueue(vmodel, queue, length / 2);
  mergeSortRenderQueue(vmodel, queue + length / 2, length - length / 2);
  uint *dst = (uint *)malloc(length * sizeof(uint));
  // order is: no mesh, no material, OPAQUE, MASK, BLEND no zwrite, BLEND zwrite
  for (uint i = 0, j = 0; i + j < length;)
  {
    int sc1 = 0, sc2 = 0;
    if (vmodel->model.nodes[queue[i]].mesh != -1)
    {
      sc1 = 10;
      for (uint k = 0; k < vmodel->model.meshes[vmodel->model.nodes[queue[i]].mesh].primitives.size(); k++)
      {
        if (vmodel->model.meshes[vmodel->model.nodes[queue[i]].mesh].primitives[k].material == -1)
          continue;
        gltf::Material &mat = vmodel->model.materials[vmodel->model.meshes[vmodel->model.nodes[queue[i]].mesh].primitives[k].material];
        int sc = mat.alphaMode;
        sc *= 10;
        if (int ext = gltf::findExtensionIndex("VRMC_materials_mtoon", mat) != -1)
        {
          sc += ((gltf::Extensions::VRMC_materials_mtoon *)mat.extensions[ext].data)->transparentWithZWrite ? 0 : (sc == 20 ? 10 : 0);
          sc += ((gltf::Extensions::VRMC_materials_mtoon *)mat.extensions[ext].data)->renderQueueOffsetNumber;
        }
        // MAX
        sc1 = sc1 > sc ? sc1 : sc;
      }
    }
    if (vmodel->model.nodes[queue[length / 2 + j]].mesh != -1)
    {
      sc2 = 10;
      for (uint k = 0; k < vmodel->model.meshes[vmodel->model.nodes[queue[length / 2 + j]].mesh].primitives.size(); k++)
      {
        if (vmodel->model.meshes[vmodel->model.nodes[queue[length / 2 + j]].mesh].primitives[k].material == -1)
          continue;
        gltf::Material &mat = vmodel->model.materials[vmodel->model.meshes[vmodel->model.nodes[queue[length / 2 + j]].mesh].primitives[k].material];
        int sc = 2 + mat.alphaMode;
        sc *= 10;
        if (int ext = gltf::findExtensionIndex("VRMC_materials_mtoon", mat) != -1)
        {
          sc += ((gltf::Extensions::VRMC_materials_mtoon *)mat.extensions[ext].data)->transparentWithZWrite ? 0 : 10;
          sc += ((gltf::Extensions::VRMC_materials_mtoon *)mat.extensions[ext].data)->renderQueueOffsetNumber;
        }
        sc2 = sc2 > sc ? sc2 : sc;
      }
    }

    if ((sc1 > sc2 || i >= length / 2) && j < length - length / 2)
    {
      dst[i + j] = queue[length / 2 + j];
      j++;
    }
    else
    {
      dst[i + j] = queue[i];
      i++;
    }
  }
  memcpy(queue, dst, sizeof(uint) * length);
  free(dst);
}
static void sortRenderQueue(VModel_t *vmodel)
{
  mergeSortRenderQueue(vmodel, vmodel->renderQueue, vmodel->model.nodes.size());
};

#define texture_base_gl_index 0
#define texture_normal_gl_index 1
#define texture_emisive_gl_index 2
#define mtoon_texture_shadeMultiply_gl_index 10
#define mtoon_texture_shadingShiftTexture_gl_index 11
#define mtoon_texture_matcapTexture_gl_index 12
#define mtoon_texture_rimMultiplyTexture_gl_index 13

void initVModel(VModel_t *vmodel)
{
  vmodel->pos = glm::vec3(0, 0, 0);

  // setup morph weights and matrix and nodeMatrix
  for (uint i = 0; i < vmodel->model.nodes.size(); i++)
  {
    gltf::Node &node = vmodel->model.nodes[i];
    if (node.weights.size() == 0)
    {
      uint maxSize = 0;
      if (node.mesh > -1)
      {
        for (gltf::Mesh::Primitive &primitive : vmodel->model.meshes[node.mesh].primitives)
        {
          maxSize = (maxSize > primitive.targets.size()) ? maxSize : primitive.targets.size();
        }
      }
      node.weights = std::vector<float>(maxSize);
      for (uint j = 0; j < node.weights.size(); j++)
      {
        node.weights[j] = 0;
      }
      if (node.mesh > -1)
      {
        vmodel->model.meshes[node.mesh].weights = std::vector<float>(maxSize);
      }
    }
  }
  // VBOs
  vmodel->VBO = (uint *)malloc(vmodel->model.bufferViews.size() * sizeof(uint));
  for (uint i = 0; i < vmodel->model.bufferViews.size(); i++)
  {
    const gltf::BufferView &bufferView = vmodel->model.bufferViews[i];
    uint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(bufferView.target, VBO);
    glBufferData(bufferView.target, bufferView.byteLength,
                 vmodel->model.buffers[bufferView.buffer].buffer + bufferView.byteOffset,
                 GL_STATIC_DRAW);
    glBindBuffer(bufferView.target, 0);
    vmodel->VBO[i] = VBO;
  }

  // node transform
  vmodel->nodeTransforms = (glm::mat4 *)malloc(vmodel->model.nodes.size() * sizeof(glm::mat4));
  for (uint i = 0; i < vmodel->model.nodes.size(); i++)
  {
    vmodel->nodeTransforms[i] = glm::mat4(1.f);
  }
  const gltf::Scene &scene = vmodel->model.scenes[vmodel->model.scene > -1 ? vmodel->model.scene : 0];
  for (uint i : scene.nodes)
  {
    updateVModelNodeTransform(vmodel, i, glm::mat4(1.f));
  }

  vmodel->renderQueue = (uint *)malloc(vmodel->model.nodes.size() * sizeof(uint));
  for (uint i = 0; i < vmodel->model.nodes.size(); i++)
  {
    vmodel->renderQueue[i] = i;
  }
  sortRenderQueue(vmodel);

  // morph targets
  vmodel->updatedMorphWeight = (bool **)malloc(sizeof(bool *) * vmodel->model.meshes.size());
  vmodel->morphs = (glm::vec3 ***)malloc(sizeof(glm::vec3 **) * vmodel->model.meshes.size());
  for (uint i = 0; i < vmodel->model.meshes.size(); i++)
  {
    vmodel->updatedMorphWeight[i] = (bool *)calloc(vmodel->model.meshes[i].weights.size(), sizeof(bool));
    vmodel->morphs[i] = (glm::vec3 **)malloc(sizeof(glm::vec3 *) * vmodel->model.meshes[i].primitives.size());
    for (uint j = 0; j < vmodel->model.meshes[i].primitives.size(); j++)
    {
      const gltf::Accessor &accessor =
          vmodel->model.accessors[vmodel->model.meshes[i].primitives[j].indices];
      vmodel->morphs[i][j] = (glm::vec3 *)calloc(accessor.count * 3, sizeof(glm::vec3));
      for (uint k = 0; k < vmodel->model.meshes[i].primitives[j].targets.size(); k++)
      {
        if (vmodel->model.meshes[i].primitives[j].targets[k].POSITION != -1)
        {
          uchar *data = gltf::getDataFromAccessor(vmodel->model, vmodel->model.accessors[vmodel->model.meshes[i].primitives[j].targets[k].POSITION]);
          glm::vec3 pos = glm::vec3(0, 0, 0);
          if (data)
            memcpy(&pos, data, sizeof(pos));
          vmodel->morphs[i][j][k] += vmodel->model.meshes[i].weights[k] * pos;
        }
        if (vmodel->model.meshes[i].primitives[j].targets[k].NORMAL != -1)
        {
          uchar *data = gltf::getDataFromAccessor(vmodel->model, vmodel->model.accessors[vmodel->model.meshes[i].primitives[j].targets[k].NORMAL]);
          glm::vec3 normal = glm::vec3(0, 0, 0);
          if (data)
            memcpy(&normal, data, sizeof(normal));
          vmodel->morphs[i][j][k + accessor.count] += vmodel->model.meshes[i].weights[k] * normal;
        }
        if (vmodel->model.meshes[i].primitives[j].targets[k].TANGENT != -1)
        {
          uchar *data = gltf::getDataFromAccessor(vmodel->model, vmodel->model.accessors[vmodel->model.meshes[i].primitives[j].targets[k].TANGENT]);
          glm::vec3 tangent = glm::vec3(0, 0, 0);
          if (data)
            memcpy(&tangent, data, sizeof(tangent));
          vmodel->morphs[i][j][k + accessor.count * 2] += vmodel->model.meshes[i].weights[k] * tangent;
        }
      }
    }
  }

  // VAO
  vmodel->VAO = (uint **)malloc(vmodel->model.meshes.size() * sizeof(uint *));
  const struct
  {
    const std::string accName;
    const int attribIndex;
  } attribs[] = {{"POSITION", 0}, {"NORMAL", 1}, {"TANGENT", 2}, {"TEXCOORD_0", 3}, {"TEXCOORD_1", 4}, {"TEXCOORD_2", 5}, {"COLOR_0", 6}, {"JOINTS_0", 7}, {"WEIGHTS_0", 8}};
  for (uint i = 0; i < vmodel->model.meshes.size(); i++)
  {
    const gltf::Mesh &mesh = vmodel->model.meshes[i];
    uint *primitivesVAO = (uint *)malloc(mesh.primitives.size() * sizeof(uint));
    for (uint j = 0; j < mesh.primitives.size(); j++)
    {
      uint VAO;
      glGenVertexArrays(1, &VAO);
      glBindVertexArray(VAO);
      const gltf::Mesh::Primitive &primitive = mesh.primitives[j];
      for (uint k = 0; k < sizeof(attribs) / sizeof(attribs[0]); k++)
      {
        if (gltf::getMeshPrimitiveAttribVal(primitive.attributes, attribs[k].accName) == -1)
          continue;
        const gltf::Accessor &accessor = vmodel->model.accessors[gltf::getMeshPrimitiveAttribVal(primitive.attributes, attribs[k].accName)];
        uint nullFilledVBO = 0;
        if (accessor.bufferView > -1)
          glBindBuffer(GL_ARRAY_BUFFER, vmodel->VBO[accessor.bufferView]);
        else
        {
          void *tmpBuffer = calloc(accessor.count, gltf::gltf_sizeof(accessor.componentType) * gltf::gltf_num_components(accessor.type));
          glGenBuffers(1, &nullFilledVBO);
          glBindBuffer(GL_ARRAY_BUFFER, nullFilledVBO);
          glBufferData(GL_ARRAY_BUFFER, accessor.count * gltf::gltf_sizeof(accessor.componentType) * gltf::gltf_num_components(accessor.type),
                       tmpBuffer,
                       GL_STATIC_DRAW);
          free(tmpBuffer);
        }

        uint attribIndex = attribs[k].attribIndex;

        uint byteStride = 0;
        if (accessor.bufferView == -1 || vmodel->model.bufferViews[accessor.bufferView].byteStride == 0)
        {
          int componentSizeInBytes = gltf::gltf_sizeof(accessor.componentType);
          int numComponents = gltf::gltf_num_components(accessor.type);
          if (componentSizeInBytes <= 0)
            byteStride = -1;
          else if (numComponents <= 0)
            byteStride = -1;
          else
            byteStride = componentSizeInBytes * numComponents;
        }
        else
        {
          int componentSizeInBytes = gltf::gltf_sizeof(accessor.componentType);
          if (componentSizeInBytes <= 0)
            byteStride = -1;
          else if ((vmodel->model.bufferViews[accessor.bufferView].byteStride % componentSizeInBytes) != 0)
            byteStride = -1;
          else
            byteStride = vmodel->model.bufferViews[accessor.bufferView].byteStride;
        }
        if (accessor.bufferView != -1)
          vmodel->model.bufferViews[accessor.bufferView].byteStride = byteStride;

        // sparse accessor
        if (accessor.sparse.count > 0)
        {
          glBindBuffer(GL_ARRAY_BUFFER, vmodel->VBO[accessor.bufferView]);
          uchar *bfIndiceData = vmodel->model.buffers[vmodel->model.bufferViews[accessor.sparse.indices.bufferView].buffer].buffer;
          bfIndiceData += vmodel->model.bufferViews[accessor.sparse.indices.bufferView].byteOffset;
          bfIndiceData += accessor.sparse.indices.byteOffset;
          for (uint l = 0; l < accessor.sparse.count; l++)
          {
            ulong sparseIndex;
            if (accessor.sparse.indices.componentType == gltf::Accessor::Sparse::Indices::UNSIGNED_BYTE)
              sparseIndex = *(((uint8_t *)bfIndiceData) + l);
            else if (accessor.sparse.indices.componentType == gltf::Accessor::Sparse::Indices::UNSIGNED_SHORT)
              sparseIndex = *(((uint16_t *)bfIndiceData) + l);
            else
              sparseIndex = *(((uint32_t *)bfIndiceData) + l);

            uchar *bfValData = vmodel->model.buffers[vmodel->model.bufferViews[accessor.sparse.values.bufferView].buffer].buffer;
            bfValData += vmodel->model.bufferViews[accessor.sparse.values.bufferView].byteOffset;
            bfValData += accessor.sparse.values.byteOffset;
            bfValData += gltf::gltf_num_components(accessor.type) * gltf::gltf_sizeof(accessor.componentType) * l;
            glBufferSubData(GL_ARRAY_BUFFER, accessor.byteOffset + sparseIndex * byteStride, gltf::gltf_sizeof(accessor.componentType) * gltf::gltf_num_components(accessor.type), bfValData);
          }
        }

        glVertexAttribPointer(attribIndex, gltf::gltf_num_components(accessor.type),
                              accessor.componentType,
                              accessor.normalized ? GL_TRUE : GL_FALSE,
                              byteStride, reinterpret_cast<void *>(accessor.byteOffset));
        glEnableVertexAttribArray(attribIndex);
        if (accessor.sparse.count > 0 && accessor.bufferView != -1)
        {
          glBufferSubData(GL_ARRAY_BUFFER, 0, vmodel->model.bufferViews[accessor.bufferView].byteLength,
                          vmodel->model.buffers[vmodel->model.bufferViews[accessor.bufferView].buffer].buffer + vmodel->model.bufferViews[accessor.bufferView].byteOffset);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        if (nullFilledVBO)
          glDeleteBuffers(1, &nullFilledVBO);
      }
      glBindVertexArray(0);
      primitivesVAO[j] = VAO;
    }
    vmodel->VAO[i] = primitivesVAO;
  }

  // textures
  vmodel->gltfImageTextureIndex = (uint *)malloc(vmodel->model.images.size() * sizeof(uint));
  for (uint i = 0; i < vmodel->model.images.size(); i++)
  {
    const gltf::Image &image = vmodel->model.images[i];
    int width, height, channels;
    uchar *im = NULL;
    if (image.bufferView != -1)
    {
      const gltf::BufferView &bufferView = vmodel->model.bufferViews[image.bufferView];
      im = stbi_load_from_memory(vmodel->model.buffers[bufferView.buffer].buffer + bufferView.byteOffset,
                                 bufferView.byteLength, &width, &height, &channels, 0);
    }
    else if (image.uri.length() != 0)
    {
      std::string directory = vmodel->path.substr(0, vmodel->path.find_last_of("/\\"));
      std::string filename = directory + "/" + image.uri;
      im = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    }
    else
    {
      width = 0;
      height = 0;
      channels = 0;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (channels == 1)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RED,
                   GL_UNSIGNED_BYTE, im);
    }
    else if (channels == 3)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, im);
    }
    else if (channels == 4)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, im);
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(im);

    vmodel->gltfImageTextureIndex[i] = tex;
  }
  glGenSamplers(1, &vmodel->sampler_obj);

  // TODO(ANT) other stuff here
}
int WORLDExecute(const gltf::glTFModel model)
{
  WORLD.FRONT = glm::vec3(0, 0, -1);
  WORLD.UP = glm::vec3(0, 1, 0);
  WORLD.RIGHT = glm::vec3(1, 0, 0);
  WORLD.camera.pos = glm::vec3(0, 0, 1);
  WORLD.camera.rot = glm::vec4(1, 0, 0, 0);
  WORLD.camera.zoom = 45;
  WORLD.lights[0] = {glm::vec3(5, 5, 5), glm::vec3(1, 1, 1), 0.3};
  WORLD.shaders.defaultShader = createShader(defaultShaderSource);
  WORLD.shaders.skeletonShader = createShader(skeletonShaderSource);
  WORLD.shaders.mtoon = createShader(mtoonShaderSource);
  return 0;
}

static void bindTexture(const VModel_t &vmodel, const Shader_t &currentShader, const gltf::Texture &texture, uint &texCoord, uint sampler_obj, uint GL_TextureIndex)
{
  if (gltf::findExtensionIndex("KHR_texture_transform", texture) != -1)
  {
    gltf::Extensions::KHR_texture_transform *ext = (gltf::Extensions::KHR_texture_transform *)texture.extensions[gltf::findExtensionIndex("KHR_texture_transform", texture)].data;
    texCoord = ext->texCoord;
    membuild(glm::vec2, offset, ext->offset);
    membuild(glm::vec2, scale, ext->scale);
    shaderSetBool(currentShader, "KHR_texture_transform", true);
    shaderSetVec2(currentShader, "KHR_texture_transform_data.u_offset", offset);
    shaderSetFloat(currentShader, "KHR_texture_transform_data.u_rotation", ext->rotation);
    shaderSetVec2(currentShader, "u_scale", scale);
  }
  else
  {
    shaderSetBool(currentShader, "KHR_texture_transform", false);
  }

  if (texture.sampler != -1)
  {
    const gltf::Sampler &sampler = vmodel.model.samplers[texture.sampler];
    glSamplerParameterf(sampler_obj, GL_TEXTURE_MIN_FILTER, sampler.minFilter);
    glSamplerParameterf(sampler_obj, GL_TEXTURE_MAG_FILTER, sampler.magFilter);
    glTexParameteri(sampler_obj, GL_TEXTURE_WRAP_S, sampler.wrapS);
    glTexParameteri(sampler_obj, GL_TEXTURE_WRAP_T, sampler.wrapT);
  }

  glActiveTexture(GL_TEXTURE0 + GL_TextureIndex);

  glBindTexture(GL_TEXTURE_2D, vmodel.gltfImageTextureIndex[texture.source]);
  glBindSampler(GL_TEXTURE_2D, sampler_obj);
}
static int renderNode(const VModel_t &vmodel, const gltf::Node &node, const glm::mat4 mat)
{
  Shader_t currentShader = WORLD.shaders.defaultShader;
  if (node.mesh > -1)
  {
    std::vector<glm::mat4> jointMatrices = std::vector<glm::mat4>();
    if (node.skin > -1)
    {
      assert(node.skin < vmodel.model.skins.size());
      const gltf::Skin &skin = vmodel.model.skins[node.skin];
      glm::mat4 nodeInverse = glm::inverse(mat);
      jointMatrices = std::vector<glm::mat4>(skin.joints.size());
      for (uint i = 0; i < skin.joints.size(); i++)
      {
        glm::mat4 nodeTransform = vmodel.nodeTransforms[skin.joints[i]];
        float *invMatrixData = (float *)gltf::getDataFromAccessor(vmodel.model, vmodel.model.accessors[skin.inverseBindMatrices], i);
        assert(invMatrixData);
        glm::mat4 inverseBindMatrix = glm::mat4(
            invMatrixData[0], invMatrixData[1], invMatrixData[2], invMatrixData[3],
            invMatrixData[4], invMatrixData[5], invMatrixData[6], invMatrixData[7],
            invMatrixData[8], invMatrixData[9], invMatrixData[10], invMatrixData[11],
            invMatrixData[12], invMatrixData[13], invMatrixData[14], invMatrixData[15]);
        jointMatrices[i] = nodeInverse * nodeTransform * inverseBindMatrix;
      }
    }
    assert(node.mesh < vmodel.model.meshes.size());
    const gltf::Mesh &mesh = vmodel.model.meshes[node.mesh];
    for (uint i = 0; i < mesh.primitives.size(); i++)
    {
      currentShader = WORLD.shaders.defaultShader;
      const gltf::Mesh::Primitive &primitive = mesh.primitives[i];
      const gltf::Accessor &indexAccessor =
          vmodel.model.accessors[primitive.indices];
      gltf::Extensions::VRMC_materials_mtoon *vrmMtoon = 0;
      // select shader
      if (primitive.material > -1)
      {
        const gltf::Material &material = vmodel.model.materials[primitive.material];
        if (gltf::findExtensionIndex("VRMC_materials_mtoon", material) != -1)
        {
          vrmMtoon = (gltf::Extensions::VRMC_materials_mtoon *)material.extensions[gltf::findExtensionIndex("VRMC_materials_mtoon", material)].data;
          currentShader = WORLD.shaders.mtoon;
        }
        else if (gltf::findExtensionIndex("VRM", vmodel.model) != -1)
        {
          currentShader = WORLD.shaders.mtoon;
        }
      }
      glUseProgram(currentShader.ID);
      shaderSetMat4(currentShader, "worldTransform", worldTransform);
      shaderSetMat4(currentShader, "node", mat);
      shaderSetMat4(currentShader, "model", modelTransform);
      shaderSetMat4Arr(currentShader, "u_jointMatrix", jointMatrices.size(), (float *)jointMatrices.data());
      glBindVertexArray(vmodel.VAO[node.mesh][i]);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vmodel.VBO[indexAccessor.bufferView]);

      bool hasBaseColorTexture = 0;
      bool doubleSided = 0;
      if (primitive.material > -1)
      {
        const gltf::Material &material = vmodel.model.materials[primitive.material];
        uint texCoord = 0;
        bool KHR_materials_unlit = !vrmMtoon && gltf::findExtensionIndex("KHR_materials_unlit", material) != -1;
        shaderSetBool(currentShader, "KHR_materials_unlit", KHR_materials_unlit);
        if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
        {
          texCoord = material.pbrMetallicRoughness.baseColorTexture.texCoord;
          hasBaseColorTexture = 1;
          const gltf::Texture &texture = vmodel.model.textures[material.pbrMetallicRoughness.baseColorTexture.index];
          shaderSetInt(currentShader, "texture_base", texture_base_gl_index);
          bindTexture(vmodel, currentShader, texture, texCoord, vmodel.sampler_obj, texture_base_gl_index);
        }
        else
        {
          glActiveTexture(GL_TEXTURE0 + texture_base_gl_index);
          glBindTexture(GL_TEXTURE_2D, 0);
        }
        if (material.pbrMetallicRoughness.baseColorFactor.size() > 0)
        {
          membuild(glm::vec4, baseColorFactor, material.pbrMetallicRoughness.baseColorFactor.data());
          shaderSetVec4(currentShader, "baseColorFactor", baseColorFactor);
        }
        else
        {
          // TODO(ANT) make sure this is correct
          shaderSetVec4(currentShader, "baseColorFactor", glm::vec4(0, 0, 0, 0));
        }
        if (material.emissiveTexture.index >= 0 && KHR_materials_unlit)
        {
          texCoord = material.emissiveTexture.texCoord;
          hasBaseColorTexture = 1;
          const gltf::Texture &texture = vmodel.model.textures[material.emissiveTexture.index];
          shaderSetInt(currentShader, "texture_emisive", texture_emisive_gl_index);
          bindTexture(vmodel, currentShader, texture, texCoord, vmodel.sampler_obj, texture_emisive_gl_index);
        }
        else
        {
          glActiveTexture(GL_TEXTURE0 + texture_emisive_gl_index);
          glBindTexture(GL_TEXTURE_2D, 0);
        }
        if (material.normalTexture.index >= 0)
        {
          texCoord = material.normalTexture.texCoord;
          const gltf::Texture &texture = vmodel.model.textures[material.normalTexture.index];
          shaderSetInt(currentShader, "texture_normal", texture_normal_gl_index);
          bindTexture(vmodel, currentShader, texture, texCoord, vmodel.sampler_obj, texture_normal_gl_index);
        }
        else
        {
          glActiveTexture(GL_TEXTURE0 + texture_normal_gl_index);
          glBindTexture(GL_TEXTURE_2D, 0);
        }

        if (material.alphaMode == gltf::Material::OPAQUE)
        {
          glDisable(GL_BLEND);
        }
        else if (material.alphaMode == gltf::Material::MASK)
        {
          glDisable(GL_BLEND);
        }
        else if (material.alphaMode == gltf::Material::BLEND)
        {
          glEnable(GL_BLEND);
        }
        doubleSided = material.doubleSided;
        shaderSetInt(currentShader, "texCoordIndex", texCoord);
        shaderSetFloat(currentShader, "alphaCutoff", material.alphaCutoff);
        if (vrmMtoon)
        {
          if (vrmMtoon->transparentWithZWrite)
            glDepthMask(false);
          else
            glDepthMask(true);
          shaderSetVec3(currentShader, "VRMData.shadeColor",
                        glm::vec3(vrmMtoon->shadeColorFactor[0], vrmMtoon->shadeColorFactor[1], vrmMtoon->shadeColorFactor[2]));
          if (vrmMtoon->shadeMultiplyTexture.index != -1)
          {
            assert(texCoord == vrmMtoon->shadeMultiplyTexture.texCoord);
            assert(vrmMtoon->shadeMultiplyTexture.index < vmodel.model.textures.size());
            const gltf::Texture &texture = vmodel.model.textures[vrmMtoon->shadeMultiplyTexture.index];
            shaderSetInt(currentShader, "VRMData.shadeMultiplyTexture", mtoon_texture_shadeMultiply_gl_index);
            bindTexture(vmodel, currentShader, texture, texCoord, vmodel.sampler_obj, mtoon_texture_shadeMultiply_gl_index);
          }
          else
          {
            glActiveTexture(GL_TEXTURE0 + mtoon_texture_shadeMultiply_gl_index);
            glBindTexture(GL_TEXTURE_2D, 0);
          }
          shaderSetFloat(currentShader, "VRMData.shadingShiftFactor", vrmMtoon->shadingShiftFactor);
          if (vrmMtoon->shadingShiftTexture.index != -1)
          {
            assert(texCoord == vrmMtoon->shadingShiftTexture.texCoord);
            const gltf::Texture &texture = vmodel.model.textures[vrmMtoon->shadingShiftTexture.index];
            shaderSetInt(currentShader, "VRMData.shadingShiftTexture", mtoon_texture_shadingShiftTexture_gl_index);
            shaderSetFloat(currentShader, "VRMData.shadingShiftTextureScale", vrmMtoon->shadingShiftTexture.scale);
            bindTexture(vmodel, currentShader, texture, texCoord, vmodel.sampler_obj, mtoon_texture_shadingShiftTexture_gl_index);
          }
          else
          {
            shaderSetFloat(currentShader, "VRMData.shadingShiftTextureScale", 0);
          }
          shaderSetFloat(currentShader, "VRMData.shadingToonyFactor", vrmMtoon->shadingToonyFactor);

          shaderSetVec3(currentShader, "VRMData.matcapFactor", glm::vec3(vrmMtoon->matcapFactor[0], vrmMtoon->matcapFactor[1], vrmMtoon->matcapFactor[2]));
          if (vrmMtoon->matcapTexture.index != -1)
          {
            assert(texCoord == vrmMtoon->matcapTexture.texCoord);
            const gltf::Texture &texture = vmodel.model.textures[vrmMtoon->matcapTexture.index];
            shaderSetInt(currentShader, "VRMData.matcapTexture", mtoon_texture_matcapTexture_gl_index);
            bindTexture(vmodel, currentShader, texture, texCoord, vmodel.sampler_obj, mtoon_texture_matcapTexture_gl_index);
          }
          else
          {
            glActiveTexture(GL_TEXTURE0 + mtoon_texture_matcapTexture_gl_index);
            glBindTexture(GL_TEXTURE_2D, 0);
          }
          shaderSetVec3(currentShader, "VRMData.parametricRimColorFactor", glm::vec3(vrmMtoon->parametricRimColorFactor[0], vrmMtoon->parametricRimColorFactor[1], vrmMtoon->parametricRimColorFactor[2]));
          shaderSetFloat(currentShader, "VRMData.parametricRimFresnelPowerFactor", vrmMtoon->parametricRimFresnelPowerFactor);
          shaderSetFloat(currentShader, "VRMData.parametricRimLiftFactor", vrmMtoon->parametricRimLiftFactor);
          if (vrmMtoon->rimMultiplyTexture.index != -1)
          {
            assert(texCoord == vrmMtoon->rimMultiplyTexture.texCoord);
            const gltf::Texture &texture = vmodel.model.textures[vrmMtoon->rimMultiplyTexture.index];
            shaderSetInt(currentShader, "VRMData.rimMultiplyTexture", mtoon_texture_rimMultiplyTexture_gl_index);
            bindTexture(vmodel, currentShader, texture, texCoord, vmodel.sampler_obj, mtoon_texture_rimMultiplyTexture_gl_index);
          }
          else
          {
            glActiveTexture(GL_TEXTURE0 + mtoon_texture_rimMultiplyTexture_gl_index);
            glBindTexture(GL_TEXTURE_2D, 0);
          }
          shaderSetFloat(currentShader, "VRMData.rimLightingMixFactor", vrmMtoon->rimLightingMixFactor);
          shaderSetInt(currentShader, "VRM_outlineWidthMode", vrmMtoon->outlineWidthMode);
          shaderSetFloat(currentShader, "VRM_outlineWidthFactor", vrmMtoon->outlineWidthFactor);
        }
      }
      shaderSetBool(currentShader, "hasBaseColorTexture", hasBaseColorTexture);
      if (doubleSided)
        glDisable(GL_CULL_FACE);
      else
        glEnable(GL_CULL_FACE);
      glUseProgram(currentShader.ID);
      glDrawElements(primitive.mode, indexAccessor.count, indexAccessor.componentType,
                     reinterpret_cast<void *>(indexAccessor.byteOffset));
    }
  }
  glBindVertexArray(0);
  return 0;
}

void drawSkeleton(const VModel_t &vmodel, glm::mat4 modelTransform, glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
  glUseProgram(WORLD.shaders.skeletonShader.ID);
  shaderSetMat4(WORLD.shaders.skeletonShader, "model", modelTransform);
  shaderSetMat4(WORLD.shaders.skeletonShader, "view", viewMatrix);
  shaderSetMat4(WORLD.shaders.skeletonShader, "projection", projectionMatrix);
  float point[3] = {0, 0, 0};
  uint VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(point), &point, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
  glUseProgram(WORLD.shaders.skeletonShader.ID);
  glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
  for (int i = 0; i < vmodel.model.nodes.size(); i++)
  {
    shaderSetMat4(WORLD.shaders.skeletonShader, "node", vmodel.nodeTransforms[i]);
    glDrawArrays(GL_POINTS, 0, 1);
  }
}

int renderVModel(const VModel_t &vmodel)
{
  glUseProgram(WORLD.shaders.defaultShader.ID);

  // model space to world space
  glm::mat4 modelT = glm::mat4(1, 0, 0, vmodel.pos.x,
                               0, 1, 0, vmodel.pos.y,
                               0, 0, 1, vmodel.pos.z,
                               0, 0, 0, 1);

  // world space to camera space
  if (WORLD.camera.updated)
  {
    WORLD.camera.viewMatrix = glm::mat4_cast(vec4ToQua(WORLD.camera.rot)) * glm::lookAt(WORLD.camera.pos, WORLD.camera.pos + WORLD.FRONT, WORLD.UP);
    WORLD.camera.projectionMatrix = glm::perspective(WORLD.camera.zoom, (float)windowData.realWindowWidth / windowData.realWindowHeight, 0.1f, 100.f);
    WORLD.camera.updated = false;
  }

  glm::mat4 viewMatrix = WORLD.camera.viewMatrix;
  glm::mat4 projectionMatrix = WORLD.camera.projectionMatrix;

  // modelDraw
  modelTransform = modelT;
  worldTransform = projectionMatrix * viewMatrix * modelT;
  for (uint i = 0; i < vmodel.model.nodes.size(); i++)
  {
    renderNode(vmodel, vmodel.model.nodes[vmodel.renderQueue[i]], vmodel.nodeTransforms[vmodel.renderQueue[i]]);
  }
  // drawSkeleton(vmodel, modelTransform, viewMatrix, projectionMatrix);

  // update pos and shit
  return 0;
}

void freeVModel(VModel_t *vmodel)
{
  glDeleteBuffers(vmodel->model.bufferViews.size(), vmodel->VBO);
  free(vmodel->VBO);
  vmodel->VBO = 0;
  for (uint i = 0; i < vmodel->model.meshes.size(); i++)
  {
    glDeleteVertexArrays(vmodel->model.meshes[i].primitives.size(), vmodel->VAO[i]);
    free(vmodel->VAO[i]);
    vmodel->VAO[i] = 0;
  }
  free(vmodel->VAO);
  vmodel->VAO = 0;
  glDeleteTextures(vmodel->model.images.size(), vmodel->gltfImageTextureIndex);
  free(vmodel->gltfImageTextureIndex);
  vmodel->gltfImageTextureIndex = 0;
  glDeleteSamplers(1, &vmodel->sampler_obj);
  gltf::freeModel(&vmodel->model);
}