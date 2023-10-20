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

#define UBO_STREAM 0
#define UBO_NODE_DYNAMIC 1
#define UBO_INVBINDMAT_STATIC 2
#define UBO_LIGHTS_STATIC 6

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
  vmodel->updatedMorphWeight = (bool *)calloc(vmodel->model.meshes.size(), sizeof(bool));
  vmodel->morphs = (glm::vec3 ***)malloc(sizeof(glm::vec3 **) * vmodel->model.meshes.size());
  vmodel->morphsVBO = (uint **)malloc(sizeof(uint *) * vmodel->model.meshes.size());
  for (uint i = 0; i < vmodel->model.meshes.size(); i++)
  {
    vmodel->morphs[i] = (glm::vec3 **)malloc(sizeof(glm::vec3 *) * vmodel->model.meshes[i].primitives.size());
    vmodel->morphsVBO[i] = (uint *)malloc(sizeof(uint) * vmodel->model.meshes[i].primitives.size());
    for (uint j = 0; j < vmodel->model.meshes[i].primitives.size(); j++)
    {
      const gltf::Mesh::Primitive &primitive = vmodel->model.meshes[i].primitives[j];
      assert(primitive.attributes.POSITION != -1);
      vmodel->morphs[i][j] = (glm::vec3 *)calloc(vmodel->model.accessors[primitive.attributes.POSITION].count * 3, sizeof(glm::vec3));
      for (uint k = 0; k < vmodel->model.meshes[i].primitives[j].targets.size(); k++)
      {
        for (uint l = 0; l < vmodel->model.accessors[primitive.attributes.POSITION].count; l++)
        {
          if (vmodel->model.meshes[i].primitives[j].targets[k].POSITION != -1)
          {
            membuild(glm::vec3, pos, gltf::getDataFromAccessor(vmodel->model, vmodel->model.accessors[vmodel->model.meshes[i].primitives[j].targets[k].POSITION], l));
            vmodel->morphs[i][j][l * 3 + 0] += vmodel->model.meshes[i].weights[k] * pos;
          }
          if (vmodel->model.meshes[i].primitives[j].targets[k].NORMAL != -1)
          {
            membuild(glm::vec3, normal, gltf::getDataFromAccessor(vmodel->model, vmodel->model.accessors[vmodel->model.meshes[i].primitives[j].targets[k].NORMAL], l));
            vmodel->morphs[i][j][l * 3 + 1] += vmodel->model.meshes[i].weights[k] * normal;
          }
          if (vmodel->model.meshes[i].primitives[j].targets[k].TANGENT != -1)
          {
            membuild(glm::vec3, tangent, gltf::getDataFromAccessor(vmodel->model, vmodel->model.accessors[vmodel->model.meshes[i].primitives[j].targets[k].TANGENT], l));
            vmodel->morphs[i][j][l * 3 + 2] += vmodel->model.meshes[i].weights[k] * tangent;
          }
        }
      }
      glGenBuffers(1, vmodel->morphsVBO[i] + j);
      glBindBuffer(GL_ARRAY_BUFFER, vmodel->morphsVBO[i][j]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vmodel->model.accessors[primitive.attributes.POSITION].count * 3, vmodel->morphs[i][j], GL_DYNAMIC_DRAW);
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
      glBindBuffer(GL_ARRAY_BUFFER, vmodel->morphsVBO[i][j]);
      glVertexAttribPointer(9, 3, GLTF_COMPONENT_FLOAT, GL_FALSE, sizeof(glm::vec3) * 3, 0);
      glEnableVertexAttribArray(9);
      glVertexAttribPointer(10, 3, GLTF_COMPONENT_FLOAT, GL_FALSE, sizeof(glm::vec3) * 3, reinterpret_cast<void *>(sizeof(glm::vec3)));
      glEnableVertexAttribArray(10);
      glVertexAttribPointer(11, 3, GLTF_COMPONENT_FLOAT, GL_FALSE, sizeof(glm::vec3) * 3, reinterpret_cast<void *>(sizeof(glm::vec3) * 2));
      glEnableVertexAttribArray(11);
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

  // UBO
  glUniformBlockBinding(WORLD.shaders.defaultShader.ID, glGetUniformBlockIndex(WORLD.shaders.defaultShader.ID, "Transforms"), UBO_STREAM);
  glUniformBlockBinding(WORLD.shaders.mtoon.ID, glGetUniformBlockIndex(WORLD.shaders.mtoon.ID, "Transforms"), UBO_STREAM);
  glUniformBlockBinding(WORLD.shaders.defaultShader.ID, glGetUniformBlockIndex(WORLD.shaders.defaultShader.ID, "Nodes"), UBO_NODE_DYNAMIC);
  glUniformBlockBinding(WORLD.shaders.mtoon.ID, glGetUniformBlockIndex(WORLD.shaders.mtoon.ID, "Nodes"), UBO_NODE_DYNAMIC);
  glUniformBlockBinding(WORLD.shaders.defaultShader.ID, glGetUniformBlockIndex(WORLD.shaders.defaultShader.ID, "InverseBindMatrices"), UBO_INVBINDMAT_STATIC);
  glUniformBlockBinding(WORLD.shaders.mtoon.ID, glGetUniformBlockIndex(WORLD.shaders.mtoon.ID, "InverseBindMatrices"), UBO_INVBINDMAT_STATIC);

  glGenBuffers(1, &vmodel->UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, vmodel->UBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2 + sizeof(int) * 4 + sizeof(int) * MAX_JOINT_MATRIX, 0, GL_STREAM_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferRange(GL_UNIFORM_BUFFER, UBO_STREAM, vmodel->UBO, 0, sizeof(glm::mat4) * 2 + sizeof(int) * 4 + sizeof(int) * MAX_JOINT_MATRIX);

  assert(vmodel->model.nodes.size() < MAX_NODES);
  glGenBuffers(1, &vmodel->nodesUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, vmodel->nodesUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * vmodel->model.nodes.size(), vmodel->nodeTransforms, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferRange(GL_UNIFORM_BUFFER, UBO_NODE_DYNAMIC, vmodel->nodesUBO, 0, sizeof(glm::mat4) * vmodel->model.nodes.size());
}
int WORLDExecute(const gltf::glTFModel model)
{
  WORLD.FRONT = glm::vec3(0, 0, -1);
  WORLD.UP = glm::vec3(0, 1, 0);
  WORLD.RIGHT = glm::vec3(1, 0, 0);
  WORLD.camera.pos = glm::vec3(0, 0, 1);
  WORLD.camera.rot = glm::vec4(1, 0, 0, 0);
  WORLD.camera.zoom = 45;
  WORLD.lights[0] = {glm::vec3(5, 5, 5), 0.3, glm::vec3(1, 1, 1)};
  WORLD.shaders.defaultShader = createShader(defaultShaderSource);
  WORLD.shaders.skeletonShader = createShader(skeletonShaderSource);
  WORLD.shaders.mtoon = createShader(mtoonShaderSource);
  glUniformBlockBinding(WORLD.shaders.defaultShader.ID, glGetUniformBlockIndex(WORLD.shaders.defaultShader.ID, "Lights"), UBO_LIGHTS_STATIC);
  glUniformBlockBinding(WORLD.shaders.mtoon.ID, glGetUniformBlockIndex(WORLD.shaders.mtoon.ID, "Lights"), UBO_LIGHTS_STATIC);

  glGenBuffers(1, &WORLD.lightsUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, WORLD.lightsUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4) * 2 * MAX_LIGHT_SOURCES, WORLD.lights, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferRange(GL_UNIFORM_BUFFER, UBO_LIGHTS_STATIC, WORLD.lightsUBO, 0, sizeof(glm::vec4) * 2 * MAX_LIGHT_SOURCES);
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
static int renderNode(const VModel_t &vmodel, const int _node, const glm::mat4 mat)
{
  const gltf::Node &node = vmodel.model.nodes[_node];
  Shader_t currentShader = WORLD.shaders.defaultShader;
  if (node.mesh > -1)
  {
    glBindBuffer(GL_UNIFORM_BUFFER, vmodel.UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, sizeof(int), &_node);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    if (node.skin > -1)
    {
      assert(node.skin < vmodel.model.skins.size());
      const gltf::Skin &skin = vmodel.model.skins[node.skin];
      assert(skin.joints.size() < MAX_JOINT_MATRIX);
      glBindBuffer(GL_UNIFORM_BUFFER, vmodel.UBO);
      glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2 + sizeof(int) * 4, sizeof(int) * skin.joints.size(), skin.joints.data());
      glBindBuffer(GL_UNIFORM_BUFFER, 0);
      glBindBufferRange(GL_UNIFORM_BUFFER, 2, vmodel.VBO[vmodel.model.accessors[skin.inverseBindMatrices].bufferView], 0, sizeof(glm::mat4) * skin.joints.size());
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

      glBindVertexArray(vmodel.VAO[node.mesh][i]);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vmodel.VBO[indexAccessor.bufferView]);

      bool hasBaseColorTexture = 0;
      bool doubleSided = 1;
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

void vmodelSetMorphWeight(VModel_t *vmodel, uint mesh, uint weight, float weightVal)
{
  vmodel->model.meshes[mesh].weights[weight] = weightVal;
  vmodel->updatedMorphWeight[mesh] = 1;
}

void vmodelVRMSetMorphWeight(VModel_t *vmodel, uint mesh, std::string target, float weight)
{
  if (gltf::findExtensionIndex("VRMC_vrm", vmodel->model) == -1)
    return;
  int index = gltf::findExtraIndex("targetNames", vmodel->model.meshes[mesh]);
  if (index < 0)
    return;
  gltf::Extras::TargetNames *targetNames = (gltf::Extras::TargetNames *)vmodel->model.extras[index].data;
  for (uint i = 0; i < targetNames->targetNames.size(); i++)
  {
    if (targetNames->targetNames[i] == target)
    {
      vmodel->model.meshes[mesh].weights[i] = weight;
      vmodel->updatedMorphWeight[mesh] = 1;
    }
  }
}

void vmodelSetVRMExpressions(VModel_t *vmodel, float *values)
{
  typedef gltf::Extensions::VRMC_vrm::ExpressionPresets ExpressionPresets;
  int index = gltf::findExtensionIndex("VRMC_vrm", vmodel->model);
  if (index < 0)
  {
    fprintf(stderr, "Model is not a VRM model\n");
    return;
  }
  gltf::Extensions::VRMC_vrm *VRM = (gltf::Extensions::VRMC_vrm *)vmodel->model.extensions[index].data;
  ExpressionPresets::Expression exp;
  int blockBlends[3] = {0, 0, 0};
  ExpressionPresets::Expression *expressionsArray = (ExpressionPresets::Expression *)&VRM->expressions.preset;
  for (int i = 0; i < sizeof(ExpressionPresets::Preset) / sizeof(ExpressionPresets::Expression); i++)
  {
    if (values[i] <= 0)
      continue;
    ExpressionPresets::Expression exp = expressionsArray[i];
    blockBlends[0] = MAX(blockBlends[0], exp.overrideBlink);
    blockBlends[1] = MAX(blockBlends[1], exp.overrideLookAt);
    blockBlends[2] = MAX(blockBlends[2], exp.overrideMouth);
  }
#define valueof(exp) values[offsetof(ExpressionPresets::Preset, exp) / sizeof(ExpressionPresets::Expression)]
#define SetMorphTargetWeight(exp, WEIGHT)                                                                                        \
  for (int i = 0; valueof(exp) > 0 && i < VRM->expressions.preset.exp.morphTargetBinds.size(); i++)                              \
  {                                                                                                                              \
    const ExpressionPresets::Expression::MorphTargetBind &morphTarget = VRM->expressions.preset.exp.morphTargetBinds[i];         \
    vmodel->updatedMorphWeight[vmodel->model.nodes[morphTarget.node].mesh] = 1;                                                  \
    vmodel->model.meshes[vmodel->model.nodes[morphTarget.node].mesh].weights[morphTarget.index] = morphTarget.weight * (WEIGHT); \
  }                                                                                                                              \
  for (int i = 0; valueof(exp) > 0 && i < VRM->expressions.preset.exp.materialColorBinds.size(); i++)                            \
  {                                                                                                                              \
    fprintf(stderr, "MaterialColorBinds not implemented yet.\n");                                                                \
  }                                                                                                                              \
  for (int i = 0; valueof(exp) > 0 && i < VRM->expressions.preset.exp.textureTransformBinds.size(); i++)                         \
  {                                                                                                                              \
    fprintf(stderr, "TextureTransformBinds not implemented yet.\n");                                                             \
  }

  // Blink
  if (blockBlends[0] != ExpressionPresets::Expression::block)
  {
    float blinkWeight = valueof(blink);
    float blinkWeightLeft = valueof(blinkLeft);
    float blinkWeightRight = valueof(blinkRight);
    blinkWeightLeft = blinkWeightLeft > 0 ? blinkWeightLeft : blinkWeight;
    blinkWeightRight = blinkWeightRight > 0 ? blinkWeightRight : blinkWeight;

    float emotionValue = 0;
    if (valueof(happy) > 0 && VRM->expressions.preset.happy.overrideBlink == ExpressionPresets::Expression::blend)
      emotionValue += valueof(happy);
    if (valueof(angry) > 0 && VRM->expressions.preset.angry.overrideBlink == ExpressionPresets::Expression::blend)
      emotionValue += valueof(angry);
    if (valueof(sad) > 0 && VRM->expressions.preset.sad.overrideBlink == ExpressionPresets::Expression::blend)
      emotionValue += valueof(sad);
    if (valueof(relaxed) > 0 && VRM->expressions.preset.relaxed.overrideBlink == ExpressionPresets::Expression::blend)
      emotionValue += valueof(relaxed);
    if (valueof(surprised) > 0 && VRM->expressions.preset.surprised.overrideBlink == ExpressionPresets::Expression::blend)
      emotionValue += valueof(surprised);
    float factor = 1 - MAX(MIN(emotionValue, 1), 0);
    blinkWeightLeft *= factor;
    blinkWeightRight *= factor;
    SetMorphTargetWeight(blinkLeft, blinkWeightLeft);
    SetMorphTargetWeight(blinkRight, blinkWeightRight);
  }
  // LookAt
  if (blockBlends[1] != ExpressionPresets::Expression::block)
  {
    float emotionValue = 0;
    if (valueof(happy) > 0 && VRM->expressions.preset.happy.overrideLookAt == ExpressionPresets::Expression::blend)
      emotionValue += valueof(happy);
    if (valueof(angry) > 0 && VRM->expressions.preset.angry.overrideLookAt == ExpressionPresets::Expression::blend)
      emotionValue += valueof(angry);
    if (valueof(sad) > 0 && VRM->expressions.preset.sad.overrideLookAt == ExpressionPresets::Expression::blend)
      emotionValue += valueof(sad);
    if (valueof(relaxed) > 0 && VRM->expressions.preset.relaxed.overrideLookAt == ExpressionPresets::Expression::blend)
      emotionValue += valueof(relaxed);
    if (valueof(surprised) > 0 && VRM->expressions.preset.surprised.overrideLookAt == ExpressionPresets::Expression::blend)
      emotionValue += valueof(surprised);
    float factor = 1 - MAX(MIN(emotionValue, 1), 0);
    SetMorphTargetWeight(lookUp, valueof(lookUp) * factor);
    SetMorphTargetWeight(lookDown, valueof(lookDown) * factor);
    SetMorphTargetWeight(lookLeft, valueof(lookLeft) * factor);
    SetMorphTargetWeight(lookRight, valueof(lookRight) * factor);
  }
  // Mouth
  if (blockBlends[2] != ExpressionPresets::Expression::block)
  {
    float emotionValue = 0;
    if (valueof(happy) > 0 && VRM->expressions.preset.happy.overrideMouth == ExpressionPresets::Expression::blend)
      emotionValue += valueof(happy);
    if (valueof(angry) > 0 && VRM->expressions.preset.angry.overrideMouth == ExpressionPresets::Expression::blend)
      emotionValue += valueof(angry);
    if (valueof(sad) > 0 && VRM->expressions.preset.sad.overrideMouth == ExpressionPresets::Expression::blend)
      emotionValue += valueof(sad);
    if (valueof(relaxed) > 0 && VRM->expressions.preset.relaxed.overrideMouth == ExpressionPresets::Expression::blend)
      emotionValue += valueof(relaxed);
    if (valueof(surprised) > 0 && VRM->expressions.preset.surprised.overrideMouth == ExpressionPresets::Expression::blend)
      emotionValue += valueof(surprised);
    float factor = 1 - MAX(MIN(emotionValue, 1), 0);
    SetMorphTargetWeight(aa, valueof(aa) * factor);
    SetMorphTargetWeight(ih, valueof(ih) * factor);
    SetMorphTargetWeight(ou, valueof(ou) * factor);
    SetMorphTargetWeight(ee, valueof(ee) * factor);
    SetMorphTargetWeight(oh, valueof(oh) * factor);
  }

  SetMorphTargetWeight(happy, valueof(happy));
  SetMorphTargetWeight(angry, valueof(angry));
  SetMorphTargetWeight(sad, valueof(sad));
  SetMorphTargetWeight(relaxed, valueof(relaxed));
  SetMorphTargetWeight(surprised, valueof(surprised));

#undef valueof
#undef SetMorphTargetWeight
}

int renderVModel(VModel_t vmodel)
{
  glUseProgram(WORLD.shaders.defaultShader.ID);

  // model space to world space
  if (WORLD.camera.updated)
  {
    WORLD.camera.viewMatrix = glm::mat4_cast(vec4ToQua(WORLD.camera.rot)) * glm::lookAt(WORLD.camera.pos, WORLD.camera.pos + WORLD.FRONT, WORLD.UP);
    WORLD.camera.projectionMatrix = glm::perspective(WORLD.camera.zoom, (float)windowData.realWindowWidth / windowData.realWindowHeight, 0.1f, 100.f);
    glm::mat4 modelTransform = glm::mat4(1, 0, 0, vmodel.pos.x,
                                         0, 1, 0, vmodel.pos.y,
                                         0, 0, 1, vmodel.pos.z,
                                         0, 0, 0, 1);

    // world space to camera space
    glm::mat4 transforms[2] = {WORLD.camera.projectionMatrix * WORLD.camera.viewMatrix * modelTransform, modelTransform};
    WORLD.camera.updated = false;

    glBindBuffer(GL_UNIFORM_BUFFER, vmodel.UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * 2, transforms);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  // modelDraw
  for (uint i = 0; i < vmodel.model.nodes.size(); i++)
  {
    renderNode(vmodel, vmodel.renderQueue[i], vmodel.nodeTransforms[vmodel.renderQueue[i]]);
  }
  // drawSkeleton(vmodel, modelTransform, viewMatrix, projectionMatrix);
  return 0;
}

int updateVModel(VModel_t *vmodel)
{
  // update morphs
  for (uint i = 0; i < vmodel->model.meshes.size(); i++)
  {
    if (vmodel->updatedMorphWeight[i])
    {
      for (uint j = 0; j < vmodel->model.meshes[i].primitives.size(); j++)
      {
        glBindBuffer(GL_ARRAY_BUFFER, vmodel->morphsVBO[i][j]);
        const gltf::Mesh::Primitive &primitive = vmodel->model.meshes[i].primitives[j];
        memset(vmodel->morphs[i][j], 0, sizeof(glm::vec3) * vmodel->model.accessors[primitive.attributes.POSITION].count * 3);
        for (uint k = 0; k < vmodel->model.meshes[i].primitives[j].targets.size(); k++)
        {
          for (uint l = 0; l < vmodel->model.accessors[primitive.attributes.POSITION].count; l++)
          {
            if (vmodel->model.meshes[i].primitives[j].targets[k].POSITION != -1)
            {
              membuild(glm::vec3, pos, gltf::getDataFromAccessor(vmodel->model, vmodel->model.accessors[vmodel->model.meshes[i].primitives[j].targets[k].POSITION], l));
              vmodel->morphs[i][j][l * 3 + 0] += vmodel->model.meshes[i].weights[k] * pos;
            }
            if (vmodel->model.meshes[i].primitives[j].targets[k].NORMAL != -1)
            {
              membuild(glm::vec3, normal, gltf::getDataFromAccessor(vmodel->model, vmodel->model.accessors[vmodel->model.meshes[i].primitives[j].targets[k].NORMAL], l));
              vmodel->morphs[i][j][l * 3 + 1] += vmodel->model.meshes[i].weights[k] * normal;
            }
            if (vmodel->model.meshes[i].primitives[j].targets[k].TANGENT != -1)
            {
              membuild(glm::vec3, tangent, gltf::getDataFromAccessor(vmodel->model, vmodel->model.accessors[vmodel->model.meshes[i].primitives[j].targets[k].TANGENT], l));
              vmodel->morphs[i][j][l * 3 + 2] += vmodel->model.meshes[i].weights[k] * tangent;
            }
          }
        }
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vmodel->model.accessors[primitive.attributes.POSITION].count * 3, vmodel->morphs[i][j]);
      }
      vmodel->updatedMorphWeight[i] = 0;
    }
  }

  // TODO(ANT) update phys

  // node transforms
  glBindBuffer(GL_UNIFORM_BUFFER, vmodel->nodesUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * vmodel->model.nodes.size(), vmodel->nodeTransforms);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

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
  for (uint i = 0; i < vmodel->model.meshes.size(); i++)
  {
    for (uint j = 0; j < vmodel->model.meshes[i].primitives.size(); j++)
    {
      free(vmodel->morphs[i][j]);
    }
    glDeleteBuffers(vmodel->model.meshes[i].primitives.size(), vmodel->morphsVBO[i]);
    free(vmodel->morphs[i]);
    free(vmodel->morphsVBO[i]);
  }
  free(vmodel->updatedMorphWeight);
  free(vmodel->morphs);
  free(vmodel->morphsVBO);
  glDeleteBuffers(1, &vmodel->UBO);
  glDeleteBuffers(1, &vmodel->nodesUBO);
  gltf::freeModel(&vmodel->model);
}