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

#define defaultShaderRelative "../src/shader/default/shader.vert", "../src/shader/default/shader.geom", "../src/shader/default/shader.frag"
#define defaultShader defaultShaderRelative
#define mtoonShader "../src/shader/MToon/MToon.vert", "../src/shader/MToon/MToon.geom", "../src/shader/MToon/MToon.frag"
#define skeletonShaderSource "../src/shader/skeletonRenderer/skeleton.vert", \
                             "../src/shader/skeletonRenderer/skeleton.geom", \
                             "../src/shader/skeletonRenderer/skeleton.frag"

WORLD_t WORLD;
Shader_t skeletonShader;

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

Shader_t createShader(std::string vertexPath, std::string geometryPath, std::string fragmentPath)
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
  glm::mat4 mat, T, R, S;
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

#define texture_base_gl_index 0
#define texture_normal_gl_index 1
#define texture_emisive_gl_index 2
void initVModel(VModel_t *vmodel)
{
  vmodel->pos = glm::vec3(0, 0, 0);
  vmodel->VAO = (uint *)malloc(vmodel->model.meshes.size() * sizeof(uint));
  vmodel->VBO = (uint *)malloc(vmodel->model.bufferViews.size() * sizeof(uint));
  vmodel->nodeTransforms = (glm::mat4 *)malloc(vmodel->model.nodes.size() * sizeof(glm::mat4));
  vmodel->gltfImageTextureIndex = (uint *)malloc(vmodel->model.images.size() * sizeof(uint));

  vmodel->shader = createShader(defaultShader);
  glUseProgram(vmodel->shader.ID);
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
  for (uint i = 0; i < vmodel->model.bufferViews.size(); i++)
  {
    const gltf::BufferView &bufferView = vmodel->model.bufferViews[i];

    int sparse_accessor = -1;
    for (uint a_i = 0; a_i < vmodel->model.accessors.size(); a_i++)
    {
      const auto &accessor = vmodel->model.accessors[a_i];
      if (accessor.bufferView == i)
      {
        if (accessor.sparse.count > 0)
        {
          sparse_accessor = a_i;
          break;
        }
      }
    }

    uint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(bufferView.target, VBO);
    if (sparse_accessor < 0)
      glBufferData(bufferView.target, bufferView.byteLength,
                   vmodel->model.buffers[bufferView.buffer].buffer + bufferView.byteOffset,
                   GL_STATIC_DRAW);
    else
    {
      const auto &accessor = vmodel->model.accessors[sparse_accessor];
      // copy the buffer to a temporary one for sparse patching
      uchar *tmp_buffer = new uchar[bufferView.byteLength];
      memcpy(tmp_buffer, vmodel->model.buffers[i].buffer + bufferView.byteOffset,
             bufferView.byteLength);

      const uint size_of_object_in_buffer =
          gltf::gltf_sizeof(accessor.componentType);
      const uint size_of_sparse_indices =
          gltf::gltf_sizeof(accessor.sparse.indices.componentType);

      const auto &indices_buffer_view =
          vmodel->model.bufferViews[accessor.sparse.indices.bufferView];
      const auto &indices_buffer = vmodel->model.buffers[indices_buffer_view.buffer];

      const auto &values_buffer_view =
          vmodel->model.bufferViews[accessor.sparse.values.bufferView];
      const auto &values_buffer = vmodel->model.buffers[values_buffer_view.buffer];

      for (uint sparse_index = 0; sparse_index < accessor.sparse.count;
           sparse_index++)
      {
        int index = 0;
        uchar *data = indices_buffer.buffer +
                      indices_buffer_view.byteOffset +
                      accessor.sparse.indices.byteOffset +
                      (sparse_index * size_of_sparse_indices);
        switch (accessor.sparse.indices.componentType)
        {
        case GLTF_COMPONENT_BYTE:
        case GLTF_COMPONENT_UBYTE:
          index = (int)*(uchar *)(data);
          break;
        case GLTF_COMPONENT_SHORT:
        case GLTF_COMPONENT_USHORT:
          index = (int)*(ushort *)(data);
          break;
        case GLTF_COMPONENT_INT:
        case GLTF_COMPONENT_UINT:
          index = (int)*(uint *)(data);
          break;
        case GLTF_COMPONENT_FLOAT:
        case GLTF_COMPONENT_DOUBLE:
        default:
          assert(0);
          break;
        }
        const uchar *read_from =
            values_buffer.buffer +
            (values_buffer_view.byteOffset +
             accessor.sparse.values.byteOffset) +
            (sparse_index * (size_of_object_in_buffer * gltf::gltf_num_components(accessor.type)));

        uchar *write_to =
            tmp_buffer + index * (size_of_object_in_buffer * gltf::gltf_num_components(accessor.type));

        memcpy(write_to, read_from, size_of_object_in_buffer * gltf::gltf_num_components(accessor.type));
      }

      glBufferData(bufferView.target, bufferView.byteLength, tmp_buffer,
                   GL_STATIC_DRAW);
      delete[] tmp_buffer;
    }
    glBindBuffer(bufferView.target, 0);

    vmodel->VBO[i] = VBO;
  }

  // node transform
  for (uint i = 0; i < vmodel->model.nodes.size(); i++)
  {
    vmodel->nodeTransforms[i] = glm::mat4(1.f);
  }
  const gltf::Scene &scene = vmodel->model.scenes[vmodel->model.scene > -1 ? vmodel->model.scene : 0];
  for (uint i : scene.nodes)
  {
    updateVModelNodeTransform(vmodel, i, glm::mat4(1.f));
  }

  // VAO
  const struct
  {
    const std::string accName;
    const int attribIndex;
  } attribs[] = {{"POSITION", 0}, {"NORMAL", 1}, {"TANGENT", 2}, {"TEXCOORD_0", 3}, {"TEXCOORD_1", 4}, {"TEXCOORD_2", 5}, {"COLOR_0", 6}, {"JOINTS_0", 7}, {"WEIGHTS_0", 8}};
  for (uint i = 0; i < vmodel->model.meshes.size(); i++)
  {

    const gltf::Mesh &mesh = vmodel->model.meshes[i];
    uint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    for (uint j = 0; j < mesh.primitives.size(); j++)
    {
      const gltf::Mesh::Primitive &primitive = mesh.primitives[j];

      for (uint k = 0; k < sizeof(attribs) / sizeof(attribs[0]); k++)
      {
        if (gltf::getMeshPrimitiveAttribVal(primitive.attributes, attribs[k].accName) == -1)
          continue;
        const gltf::Accessor &accessor = vmodel->model.accessors[gltf::getMeshPrimitiveAttribVal(primitive.attributes, attribs[k].accName)];
        glBindBuffer(GL_ARRAY_BUFFER, vmodel->VBO[accessor.bufferView]);

        uint attribIndex = attribs[k].attribIndex;

        uint byteStride = 0;
        if (vmodel->model.bufferViews[accessor.bufferView].byteStride == 0)
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
        vmodel->model.bufferViews[accessor.bufferView].byteStride = byteStride;

        glVertexAttribPointer(attribIndex, gltf::gltf_num_components(accessor.type),
                              accessor.componentType,
                              accessor.normalized ? GL_TRUE : GL_FALSE,
                              byteStride, reinterpret_cast<void *>(accessor.byteOffset));
        glEnableVertexAttribArray(attribIndex);
      }
    }
    vmodel->VAO[i] = VAO;
  }

  // textures
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
      glDeleteBuffers(1, vmodel->VBO + image.bufferView);
      vmodel->VBO[image.bufferView] = 0;

      // NOTE(ANT) why doesnt this work??
      // glTexBuffer(GL_TEXTURE_2D, GL_RGB, gltfBufferViewVBO[image.bufferView]);
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
  shaderSetInt(vmodel->shader, "texture_base", texture_base_gl_index);
  shaderSetInt(vmodel->shader, "texture_normal", texture_normal_gl_index);
  shaderSetInt(vmodel->shader, "texture_emisive", texture_emisive_gl_index);

  // TODO(ANT) other stuff here
}
int WORLDExecute(const gltf::glTFModel model)
{
  WORLD.FRONT = glm::vec3(-1, 0, 0);
  WORLD.UP = glm::vec3(0, 1, 0);
  WORLD.RIGHT = glm::vec3(0, 0, 1);
  WORLD.camera.pos = glm::vec3(1, 0, 0);
  WORLD.camera.rot = glm::vec4(1, 0, 0, 0);
  WORLD.camera.zoom = 45;
  WORLD.lights[0] = {glm::vec3(0, 5, -5), glm::vec3(1, 1, 1), 0.2};
  skeletonShader = createShader(skeletonShaderSource);
  return 0;
}

static void bindTexture(const VModel_t &vmodel, const gltf::Texture &texture, uint &texCoord, uint sampler_obj, uint GL_TextureIndex)
{
  if (gltf::findExtensionIndex("KHR_texture_transform", texture) != -1)
  {
    gltf::Extensions::KHR_texture_transform *ext = (gltf::Extensions::KHR_texture_transform *)texture.extensions[gltf::findExtensionIndex("KHR_texture_transform", texture)].data;
    texCoord = ext->texCoord;
    membuild(glm::vec2, offset, ext->offset);
    membuild(glm::vec2, scale, ext->scale);
    shaderSetBool(vmodel.shader, "KHR_texture_transform", true);
    shaderSetVec2(vmodel.shader, "KHR_texture_transform_data.u_offset", offset);
    shaderSetFloat(vmodel.shader, "KHR_texture_transform_data.u_rotation", ext->rotation);
    shaderSetVec2(vmodel.shader, "u_scale", scale);
  }
  else
  {
    shaderSetBool(vmodel.shader, "KHR_texture_transform", false);
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
static int renderNode(const VModel_t &vmodel, const gltf::Node &node, glm::mat4 mat)
{
  if (node.mesh > -1)
  {
    if (node.skin > -1)
    {
      assert(node.skin < vmodel.model.skins.size());
      const gltf::Skin &skin = vmodel.model.skins[node.skin];
      glm::mat4 nodeInverse = glm::inverse(mat);
      std::vector<glm::mat4> jointMatrices = std::vector<glm::mat4>(skin.joints.size());
      for (uint i = 0; i < skin.joints.size(); i++)
      {
        glm::mat4 nodeTransform = vmodel.nodeTransforms[skin.joints[i]];
        float*invMatrixData=(float *)gltf::getDataFromAccessor(vmodel.model, vmodel.model.accessors[skin.inverseBindMatrices], i);
        glm::mat4 inverseBindMatrix = glm::mat4(
            invMatrixData[0], invMatrixData[1], invMatrixData[2], invMatrixData[3],
            invMatrixData[4], invMatrixData[5], invMatrixData[6], invMatrixData[7],
            invMatrixData[8], invMatrixData[9], invMatrixData[10], invMatrixData[11],
            invMatrixData[12], invMatrixData[13], invMatrixData[14], invMatrixData[15]);
        jointMatrices[i] = nodeInverse * nodeTransform * inverseBindMatrix;
      }
      shaderSetMat4Arr(vmodel.shader, "u_jointMatrix", jointMatrices.size(), (float *)jointMatrices.data());
    }
    shaderSetMat4(vmodel.shader, "node", mat);
    assert(node.mesh < vmodel.model.meshes.size());
    const gltf::Mesh &mesh = vmodel.model.meshes[node.mesh];
    glBindVertexArray(vmodel.VAO[node.mesh]);
    uint sampler_obj;
    glGenSamplers(1, &sampler_obj);
    for (uint i = 0; i < mesh.primitives.size(); i++)
    {
      const gltf::Mesh::Primitive &primitive = mesh.primitives[i];
      const gltf::Accessor &indexAccessor =
          vmodel.model.accessors[primitive.indices];
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vmodel.VBO[indexAccessor.bufferView]);

      bool hasBaseColorTexture = 0;
      bool doubleSided = 0;
      if (primitive.material > -1)
      {
        const gltf::Material &material = vmodel.model.materials[primitive.material];
        uint texCoord = 0;

        bool KHR_materials_unlit = vmodel.vrmData || gltf::findExtensionIndex("KHR_materials_unlit", material) != -1;

        if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
        {
          texCoord = material.pbrMetallicRoughness.baseColorTexture.texCoord;
          hasBaseColorTexture = 1;
          const gltf::Texture &texture = vmodel.model.textures[material.pbrMetallicRoughness.baseColorTexture.index];
          bindTexture(vmodel, texture, texCoord, sampler_obj, texture_base_gl_index);
        }
        if (material.pbrMetallicRoughness.baseColorFactor.size() > 0)
        {
          glm::vec4 baseColorFactor;
          memcpy(&baseColorFactor, material.pbrMetallicRoughness.baseColorFactor.data(), sizeof(float) * 4);
          shaderSetVec4(vmodel.shader, "baseColorFactor", baseColorFactor);
        }
        if (material.emissiveTexture.index >= 0 && KHR_materials_unlit)
        {
          texCoord = material.emissiveTexture.texCoord;
          hasBaseColorTexture = 1;
          const gltf::Texture &texture = vmodel.model.textures[material.emissiveTexture.index];
          bindTexture(vmodel, texture, texCoord, sampler_obj, texture_emisive_gl_index);
        }
        if (material.normalTexture.index >= 0)
        {
          texCoord = material.normalTexture.texCoord;
          const gltf::Texture &texture = vmodel.model.textures[material.normalTexture.index];
          bindTexture(vmodel, texture, texCoord, sampler_obj, texture_normal_gl_index);
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
        shaderSetInt(vmodel.shader, "texCoordIndex", texCoord);
        // TODO(ANT) VRM HERE
        shaderSetFloat(vmodel.shader, "alphaCutoff", material.alphaCutoff);
      }
      shaderSetBool(vmodel.shader, "hasBaseColorTexture", hasBaseColorTexture);
      if (doubleSided)
        glDisable(GL_CULL_FACE);
      else
        glEnable(GL_CULL_FACE);

      glDrawElements(primitive.mode, indexAccessor.count, indexAccessor.componentType,
                     reinterpret_cast<void *>(indexAccessor.byteOffset));
    }
    glDeleteSamplers(1, &sampler_obj);
  }
  for (uint i : node.children)
  {
    int r = renderNode(vmodel, vmodel.model.nodes[i], vmodel.nodeTransforms[i]);
    if (r)
      return r;
  }
  return 0;
}

void drawSkeleton(const VModel_t &vmodel, glm::mat4 modelTransform, glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
  glUseProgram(skeletonShader.ID);
  shaderSetMat4(skeletonShader, "model", modelTransform);
  shaderSetMat4(skeletonShader, "view", viewMatrix);
  shaderSetMat4(skeletonShader, "projection", projectionMatrix);
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
  glUseProgram(skeletonShader.ID);
  glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
  for (int i = 0; i < vmodel.model.nodes.size(); i++)
  {
    shaderSetMat4(skeletonShader, "node", vmodel.nodeTransforms[i]);
    glDrawArrays(GL_POINTS, 0, 1);
  }
}

int renderVModel(const VModel_t &vmodel)
{
  glUseProgram(vmodel.shader.ID);
  // TODO(ANT) see if necessary to pass light every frame, maybe only do it once
  for (int i = 0; i < MAX_LIGHT_SOURCES; i++)
  {
    char lightName[sizeof("lights[000]")] = {0};
    sprintf(lightName, "lights[%d]", i);
    shaderSetVec3(vmodel.shader, std::string(lightName) + ".Position", WORLD.lights[i].pos);
    shaderSetVec3(vmodel.shader, std::string(lightName) + ".Color", WORLD.lights[i].color);
    shaderSetFloat(vmodel.shader, std::string(lightName) + ".Intensity", WORLD.lights[i].intensity);

    // shaderSetFloatArr(WORLD.shader, std::string(lightName), 7, (float *)&WORLD.lights[i]);
  }

  // model space to world space
  glm::mat4 modelTransform = glm::mat4(1);
  modelTransform = glm::translate(modelTransform, vmodel.pos);
  shaderSetMat4(vmodel.shader, "model", modelTransform);
  // world space to camera space
  glm::mat4 viewMatrix = glm::lookAt(WORLD.camera.pos, WORLD.camera.pos + WORLD.FRONT, WORLD.UP);
  shaderSetMat4(vmodel.shader, "view", viewMatrix);
  // camera space to display space
  glm::mat4 projectionMatrix = glm::mat4_cast(vec4ToQua(WORLD.camera.rot)) * glm::perspective(WORLD.camera.zoom, (float)windowData.realWindowWidth / windowData.realWindowHeight, 0.1f, 100.f);
  shaderSetMat4(vmodel.shader, "projection", projectionMatrix);

  // modelDraw
  const gltf::Scene &scene = vmodel.model.scenes[vmodel.model.scene > -1 ? vmodel.model.scene : 0];
  // TODO(ANT) maybe need to loop for each material type
  for (uint i : scene.nodes)
  {
    renderNode(vmodel, vmodel.model.nodes[i], glm::mat4(1));
  }

  // drawSkeleton(vmodel, modelTransform, viewMatrix, projectionMatrix);

  // update pos and shit
  return 0;
}
