#include "loader.hpp"
#include <nlohmann/json.hpp>
#include <chevan_utils_print.hpp>

#define _toStr(a) #a
#define toStr(a) _toStr(a)
typedef nlohmann::json json;
struct BIN_t
{
  uchar *data;
  size_t size;
};

static uchar *pop_buff(uchar **data, size_t length)
{
  uchar *a = *data;
  *data += length;
  return a;
}
#define pop_buff(data, length) pop_buff((uchar **)&(data), (length))

static std::vector<gltf::Extension> deserialize_extensions(json extensions)
{
  std::vector<gltf::Extension> vec;
  for (auto x : extensions.items())
  {
    gltf::Extension ext;
    ext.name = x.key();
    if (ext.name == gltf::SUPPORTED_EXTENSIONS.KHR_materials_unlit)
    {
      // ext.data = new gltf::Extensions::KHR_materials_unlit();
    }
    else if (ext.name == gltf::SUPPORTED_EXTENSIONS.KHR_texture_transform)
    {
      // gltf::Extensions::KHR_texture_transform *transform = new gltf::Extensions::KHR_texture_transform();
      // transform->offset = std::vector<float>(x.value()["offset"].size());
      // for (uint i = 0; i < x.value()["offset"].size(); i++)
      // {
      //   transform->offset[i] = x.value()["offset"][i].get<float>();
      // }
      // transform->rotation = x.value()["rotation"].get<float>();
      // transform->scale = std::vector<float>(x.value()["scale"].size());
      // for (uint i = 0; i < x.value()["scale"].size(); i++)
      // {
      //   transform->scale[i] = x.value()["scale"][i].get<float>();
      // }
      // transform->texCoord = x.value()["texCoord"].get<int>();
      // ext.data = transform;
    }
    else if (ext.name == gltf::SUPPORTED_EXTENSIONS.VRM)
    {
      // gltf::Extensions::VRM tmpExt = deserializeVRM(x.value());
      // gltf::Extensions::VRM *vrm = new gltf::Extensions::VRM();
      // *vrm = tmpExt;
      // ext.data = vrm;
    }
    else
    {
      fprintf(stderr, "%s: extension not supported", x.key().c_str());
      exit(1);
    }
    vec.push_back(ext);
  }
  return vec;
}
static std::vector<gltf::Extension> deserialize_extras(json extras)
{
  return std::vector<gltf::Extension>();
}

static gltf::glTFModel parseGLTFJSON(json glTF_data)
{
#define jsonCondition(item, id) if (std::string(item.key()) == #id)
#define jsonFunctionMacro(item, id, func) \
  jsonCondition(item, id)                 \
  {                                       \
    func;                                 \
    continue;                             \
  }
#define jsonExtensionMacro(item, dst, id) jsonFunctionMacro(item, id, dst.id = deserialize_##id(item.value());)
#define jsonConvertMacro(item, dst, id, type) jsonFunctionMacro(item, id, dst.id = item.value().get<type>();)
#define jsonConvertCastMacro(item, dst, id, type, castType) jsonFunctionMacro(item, id, dst.id = (castType)item.value().get<type>();)
#define jsonArrayMacro(item, dst, id, type, i) jsonFunctionMacro( \
    item, id, dst.id.resize(item.value().size()); for (uint i = 0; i < item.value().size(); i++) { dst.id[i] = item.value()[i].get<type>(); });
#define jsonUnknownMacro(item, location) println(__FILE__, ":", __LINE__, " ", item.key(), ": unknow item in object ", #location);

  gltf::glTFModel gltf;

  // parse json in object
  for (auto &item : glTF_data.items())
  {
    jsonCondition(item, asset)
    {
      for (auto &x : glTF_data["asset"].items())
      {
        jsonConvertMacro(x, gltf.asset, version, std::string);
        jsonConvertMacro(x, gltf.asset, generator, std::string);
        jsonConvertMacro(x, gltf.asset, copyright, std::string);
        jsonConvertMacro(x, gltf.asset, minVersion, std::string);
        jsonExtensionMacro(x, gltf.asset, extensions);
        jsonExtensionMacro(x, gltf.asset, extras);
        jsonUnknownMacro(x, model.asset);
      }
      continue;
    }
    jsonCondition(item, buffers)
    {
      for (uint i = 0; i < glTF_data["buffers"].size(); i++)
      {
        gltf::Buffer buffer;
        for (auto &x : glTF_data["buffers"][i].items())
        {
          jsonConvertMacro(x, buffer, uri, std::string);
          jsonConvertMacro(x, buffer, byteLength, int);
          jsonConvertMacro(x, buffer, name, std::string);
          jsonExtensionMacro(x, buffer, extensions);
          jsonExtensionMacro(x, buffer, extras);
          jsonUnknownMacro(x, model.buffers);
        }
        gltf.buffers.push_back(buffer);
      }
      continue;
    }
    jsonCondition(item, bufferViews)
    {
      for (uint i = 0; i < glTF_data["bufferViews"].size(); i++)
      {
        gltf::BufferView bufferV;
        for (auto &x : glTF_data["bufferViews"][i].items())
        {
          jsonConvertMacro(x, bufferV, buffer, int);
          jsonConvertMacro(x, bufferV, byteOffset, int);
          jsonConvertMacro(x, bufferV, byteLength, int);
          jsonConvertMacro(x, bufferV, byteStride, int);
          jsonConvertMacro(x, bufferV, target, int);
          jsonConvertMacro(x, bufferV, name, std::string);
          jsonExtensionMacro(x, bufferV, extensions);
          jsonExtensionMacro(x, bufferV, extras);
          jsonUnknownMacro(x, model.bufferViews);
        }
        gltf.bufferViews.push_back(bufferV);
      }
      continue;
    }
    jsonCondition(item, accessors)
    {
      for (uint i = 0; i < glTF_data["accessors"].size(); i++)
      {
        gltf::Accessor accessor;
        for (auto &x : glTF_data["accessors"][i].items())
        {
          jsonConvertMacro(x, accessor, bufferView, int);
          jsonConvertMacro(x, accessor, byteOffset, int);
          if (std::string(x.key()) == "type")
          {
            if (x.value().get<std::string>() == "SCALAR")
            {
              accessor.type = gltf::Accessor::Types::SCALAR;
            }
            else if (x.value().get<std::string>() == "VEC2")
            {
              accessor.type = gltf::Accessor::Types::VEC2;
            }
            else if (x.value().get<std::string>() == "VEC3")
            {
              accessor.type = gltf::Accessor::Types::VEC3;
            }
            else if (x.value().get<std::string>() == "VEC4")
            {
              accessor.type = gltf::Accessor::Types::VEC4;
            }
            else if (x.value().get<std::string>() == "MAT2")
            {
              accessor.type = gltf::Accessor::Types::MAT2;
            }
            else if (x.value().get<std::string>() == "MAT3")
            {
              accessor.type = gltf::Accessor::Types::MAT3;
            }
            else if (x.value().get<std::string>() == "MAT4")
            {
              accessor.type = gltf::Accessor::Types::MAT4;
            }
            continue;
          }
          jsonConvertCastMacro(x, accessor, componentType, int, gltf::Accessor::glComponentType);
          jsonConvertMacro(x, accessor, count, int);
          jsonArrayMacro(x, accessor, max, float, j);
          jsonArrayMacro(x, accessor, min, float, j);
          jsonConvertMacro(x, accessor, normalized, bool);
          if (std::string(x.key()) == "sparse")
          {
            jsonConvertMacro(x, accessor.sparse, count, int);
            if (std::string(x.key()) == "indices")
            {
              for (auto &y : x.value().items())
              {
                jsonConvertMacro(y, accessor.sparse.indices, bufferView, int);
                jsonConvertMacro(y, accessor.sparse.indices, byteOffset, int);
                jsonConvertCastMacro(y, accessor.sparse.indices, componentType, int, gltf::Accessor::Sparse::Indices::glComponentType)
                    jsonExtensionMacro(y, accessor.sparse.indices, extensions);
                jsonExtensionMacro(y, accessor.sparse.indices, extras);
              }
            }
            if (std::string(x.key()) == "values")
            {
              for (auto &y : x.value().items())
              {
                jsonConvertMacro(y, accessor.sparse.values, bufferView, int);
                jsonConvertMacro(y, accessor.sparse.values, byteOffset, int);
                jsonExtensionMacro(y, accessor.sparse.values, extensions);
                jsonExtensionMacro(y, accessor.sparse.values, extras);
              }
            }
            jsonExtensionMacro(x, accessor.sparse, extensions);
            jsonExtensionMacro(x, accessor.sparse, extras);
            jsonUnknownMacro(x, model.accessors.sparse) continue;
          }
          jsonConvertMacro(x, accessor, name, std::string);
          jsonExtensionMacro(x, accessor, extensions);
          jsonExtensionMacro(x, accessor, extras);
          jsonUnknownMacro(x, model.accessors);
        }
        gltf.accessors.push_back(accessor);
      }
      continue;
    }
    jsonCondition(item, textures)
    {
      for (uint i = 0; i < glTF_data["textures"].size(); i++)
      {
        gltf::Texture texture;
        for (auto &x : glTF_data["textures"][i].items())
        {
          jsonConvertMacro(x, texture, sampler, int);
          jsonConvertMacro(x, texture, source, int);
          jsonConvertMacro(x, texture, name, std::string);
          jsonExtensionMacro(x, texture, extensions);
          jsonExtensionMacro(x, texture, extras);
          jsonUnknownMacro(x, model.textures);
        }
        gltf.textures.push_back(texture);
      }
      continue;
    }
    jsonCondition(item, samplers)
    {
      for (uint i = 0; i < glTF_data["samplers"].size(); i++)
      {
        gltf::Sampler sampler;
        for (auto &x : glTF_data["samplers"][i].items())
        {
          jsonConvertCastMacro(x, sampler, magFilter, int, gltf::Sampler::glFilter);
          jsonConvertCastMacro(x, sampler, minFilter, int, gltf::Sampler::glFilter);
          jsonConvertCastMacro(x, sampler, wrapS, int, gltf::Sampler::glWrap);
          jsonConvertCastMacro(x, sampler, wrapT, int, gltf::Sampler::glWrap);
          jsonConvertMacro(x, sampler, name, std::string);
          jsonExtensionMacro(x, sampler, extensions);
          jsonExtensionMacro(x, sampler, extras);
          jsonUnknownMacro(x, model.samplers);
        }
        gltf.samplers.push_back(sampler);
      }
      continue;
    }
    jsonCondition(item, images)
    {
      for (uint i = 0; i < glTF_data["images"].size(); i++)
      {
        gltf::Image image;
        for (auto &x : glTF_data["images"][i].items())
        {
          jsonConvertMacro(x, image, name, std::string);
          jsonConvertMacro(x, image, uri, std::string);
          jsonConvertMacro(x, image, bufferView, int);
          jsonConvertMacro(x, image, mimeType, std::string);
          jsonExtensionMacro(x, image, extensions);
          jsonExtensionMacro(x, image, extras);
          jsonUnknownMacro(x, model.images);
        }
        gltf.images.push_back(image);
      }
      continue;
    }
    jsonCondition(item, materials)
    {
      for (uint i = 0; i < glTF_data["materials"].size(); i++)
      {
        gltf::Material material;
        for (auto &x : glTF_data["materials"][i].items())
        {
          jsonConvertMacro(x, material, name, std::string);
          if (std::string(x.key()) == "pbrMetallicRoughness")
          {
            for (auto &y : x.value().items())
            {
              if (std::string(y.key()) == "baseColorTexture")
              {
                for (auto &z : y.value().items())
                {
                  jsonConvertMacro(z, material.pbrMetallicRoughness.baseColorTexture, index, int);
                  jsonConvertMacro(z, material.pbrMetallicRoughness.baseColorTexture, texCoord, int);
                  jsonExtensionMacro(z, material.pbrMetallicRoughness.baseColorTexture, extensions);
                  jsonExtensionMacro(z, material.pbrMetallicRoughness.baseColorTexture, extras);
                  jsonUnknownMacro(z, model.materials.pbrMetallicRoughness.baseColorTexture);
                }
                continue;
              }
              if (std::string(y.key()) == "baseColorFactor")
              {
                std::vector<float> baseColorFactor;
                for (uint j = 0; j < y.value().size(); j++)
                {
                  baseColorFactor.push_back(y.value()[j].get<float>());
                }
                material.pbrMetallicRoughness.baseColorFactor = baseColorFactor;
                continue;
              }
              if (std::string(y.key()) == "metallicRoughnessTexture")
              {
                for (auto &z : y.value().items())
                {
                  jsonConvertMacro(z, material.pbrMetallicRoughness.metallicRoughnessTexture, index, int);
                  jsonConvertMacro(z, material.pbrMetallicRoughness.metallicRoughnessTexture, texCoord, int);
                  jsonExtensionMacro(z, material.pbrMetallicRoughness.metallicRoughnessTexture, extensions);
                  jsonExtensionMacro(z, material.pbrMetallicRoughness.metallicRoughnessTexture, extras);
                  jsonUnknownMacro(z, model.materials.pbrMetallicRoughness.metallicRoughnessTexture);
                }
                continue;
              }
              jsonConvertMacro(y, material.pbrMetallicRoughness, metallicFactor, float);
              jsonConvertMacro(y, material.pbrMetallicRoughness, roughnessFactor, float);
              jsonExtensionMacro(y, material.pbrMetallicRoughness, extensions);
              jsonExtensionMacro(y, material.pbrMetallicRoughness, extras);
              jsonUnknownMacro(y, model.materials.pbrMetallicRoughness);
            }
            continue;
          }
          if (std::string(x.key()) == "normalTexture")
          {
            for (auto &y : x.value().items())
            {
              jsonConvertMacro(y, material.normalTexture, scale, float);
              jsonConvertMacro(y, material.normalTexture, index, int);
              jsonConvertMacro(y, material.normalTexture, texCoord, int);
              jsonExtensionMacro(y, material.normalTexture, extensions);
              jsonExtensionMacro(y, material.normalTexture, extras);
              jsonUnknownMacro(y, model.materials.normalTexture);
            }
            continue;
          }
          if (std::string(x.key()) == "occlusionTexture")
          {
            for (auto &y : x.value().items())
            {
              jsonConvertMacro(y, material.occlusionTexture, strength, float);
              jsonConvertMacro(y, material.occlusionTexture, index, int);
              jsonConvertMacro(y, material.occlusionTexture, texCoord, int);
              jsonExtensionMacro(y, material.occlusionTexture, extensions);
              jsonExtensionMacro(y, material.occlusionTexture, extras);
              jsonUnknownMacro(y, model.materials.occlusionTexture);
            }
            continue;
          }
          if (std::string(x.key()) == "emissiveTexture")
          {
            for (auto &y : x.value().items())
            {
              jsonConvertMacro(y, material.emissiveTexture, index, int);
              jsonConvertMacro(y, material.emissiveTexture, texCoord, int);
              jsonExtensionMacro(y, material.emissiveTexture, extensions);
              jsonExtensionMacro(y, material.emissiveTexture, extras);
              jsonUnknownMacro(y, model.materials.emisiveTexture);
            }
            continue;
          }
          jsonArrayMacro(x, material, emissiveFactor, float, j);
          // if (std::string(x.key()) == "emissiveFactor")
          // {
          //   std::vector<float> emissiveFactor;
          //   for (uint j = 0; j < x.value().size(); j++)
          //   {
          //     emissiveFactor.push_back(x.value()[j]);
          //   }
          //   material.emissiveFactor = emissiveFactor;
          //   continue;
          // }
          if (std::string(x.key()) == "alphaMode")
          {
            if (x.value().get<std::string>() == "OPAQUE")
            {
              material.alphaMode = gltf::Material::AlphaMode::OPAQUE;
            }
            else if (x.value().get<std::string>() == "MASK")
            {
              material.alphaMode = gltf::Material::AlphaMode::MASK;
            }
            else if (x.value().get<std::string>() == "BLEND")
            {
              material.alphaMode = gltf::Material::AlphaMode::BLEND;
            }
            continue;
          }
          jsonConvertMacro(x, material, alphaCutoff, float);
          jsonConvertMacro(x, material, doubleSided, bool);
          jsonExtensionMacro(x, material, extensions);
          jsonExtensionMacro(x, material, extras);
          jsonUnknownMacro(x, model.materials);
        }
        gltf.materials.push_back(material);
      }
      continue;
    }
    jsonCondition(item, meshes)
    {
      for (uint i = 0; i < glTF_data["meshes"].size(); i++)
      {
        gltf::Mesh mesh;
        for (auto &x : glTF_data["meshes"][i].items())
        {
          jsonConvertMacro(x, mesh, name, std::string);
          if (std::string(x.key()) == "primitives")
          {
            for (uint j = 0; j < x.value().size(); j++)
            {
              gltf::Mesh::Primitive primitive;
              for (auto &y : x.value()[j].items())
              {
                jsonConvertMacro(y, primitive, mode, int);
                jsonConvertMacro(y, primitive, indices, int);
                if (std::string(y.key()) == "attributes")
                {
                  for (auto &z : y.value().items())
                  {
                    jsonConvertMacro(z, primitive.attributes, POSITION, int);
                    jsonConvertMacro(z, primitive.attributes, NORMAL, int);
                    jsonConvertMacro(z, primitive.attributes, TANGENT, int);
                    jsonConvertMacro(z, primitive.attributes, TEXCOORD_0, int);
                    jsonConvertMacro(z, primitive.attributes, TEXCOORD_1, int);
                    jsonConvertMacro(z, primitive.attributes, COLOR_0, int);
                    jsonConvertMacro(z, primitive.attributes, JOINTS_0, int);
                    jsonConvertMacro(z, primitive.attributes, WEIGHTS_0, int);
                    jsonUnknownMacro(z, model.meshes.primitives.attributes)
                  }
                  continue;
                }
                jsonConvertMacro(y, primitive, material, int);
                if (std::string(y.key()) == "targets")
                {
                  for (uint k = 0; k < y.value().size(); k++)
                  {
                    gltf::Mesh::Primitive::MorphTarget target;
                    for (auto &z : y.value()[k].items())
                    {
                      jsonConvertMacro(z, target, POSITION, int);
                      jsonConvertMacro(z, target, NORMAL, int);
                      jsonConvertMacro(z, target, TANGENT, int);
                      jsonUnknownMacro(z, model.meshes.primitives.targets);
                    }
                    primitive.targets.push_back(target);
                  }
                  continue;
                }
                jsonExtensionMacro(y, primitive, extensions);
                jsonExtensionMacro(y, primitive, extras);
                jsonUnknownMacro(y, model.meshes.primitives);
              }
              mesh.primitives.push_back(primitive);
            }
            continue;
          }
          jsonArrayMacro(x, mesh, weights, float, j);
          jsonExtensionMacro(x, mesh, extensions);
          jsonExtensionMacro(x, mesh, extras);
          jsonUnknownMacro(x, model.meshes);
        }
        gltf.meshes.push_back(mesh);
      }
      continue;
    }
    jsonCondition(item, nodes)
    {
      for (uint i = 0; i < glTF_data["nodes"].size(); i++)
      {
        gltf::Node node;
        for (auto &x : glTF_data["nodes"][i].items())
        {
          jsonConvertMacro(x, node, name, std::string);
          jsonArrayMacro(x, node, children, int, j);
          jsonArrayMacro(x, node, matrix, float, j);
          jsonArrayMacro(x, node, translation, float, j);
          jsonArrayMacro(x, node, rotation, float, j);
          jsonArrayMacro(x, node, scale, float, j);
          jsonConvertMacro(x, node, mesh, int);
          jsonConvertMacro(x, node, skin, int);
          jsonArrayMacro(x, node, weights, float, j);
          jsonConvertMacro(x, node, camera, int);
          jsonExtensionMacro(x, node, extensions);
          jsonExtensionMacro(x, node, extras);
          jsonUnknownMacro(x, model.nodes);
        }
        if (node.matrix.size() == 0)
        {
          // node.matrix = std::vector<float>(
          //     {1, 0, 0, 0,
          //      0, 1, 0, 0,
          //      0, 0, 1, 0,
          //      0, 0, 0, 1});
        }
        gltf.nodes.push_back(node);
      }
      continue;
    }
    jsonCondition(item, skins)
    {
      for (uint i = 0; i < glTF_data["skins"].size(); i++)
      {
        gltf::Skin skin;
        for (auto &x : glTF_data["skins"][i].items())
        {
          jsonConvertMacro(x, skin, name, std::string);
          jsonConvertMacro(x, skin, inverseBindMatrices, int);
          jsonArrayMacro(x, skin, joints, int, j);
          jsonConvertMacro(x, skin, skeleton, int);
          jsonExtensionMacro(x, skin, extensions);
          jsonExtensionMacro(x, skin, extras);
          jsonUnknownMacro(x, model.skins);
        }
        gltf.skins.push_back(skin);
      }
      continue;
    }
    jsonCondition(item, scene)
    {
      gltf.scene = glTF_data["scene"].get<int>();
      continue;
    }
    jsonCondition(item, scenes)
    {
      for (uint i = 0; i < glTF_data["scenes"].size(); i++)
      {
        gltf::Scene scene;
        for (auto &x : glTF_data["scenes"][i].items())
        {
          jsonConvertMacro(x, scene, name, std::string);
          jsonArrayMacro(x, scene, nodes, int, j);
          jsonExtensionMacro(x, scene, extensions);
          jsonExtensionMacro(x, scene, extras);
          jsonUnknownMacro(x, model.scene);
        }
        gltf.scenes.push_back(scene);
      }
      continue;
    }
    jsonCondition(item, animations)
    {
      for (uint i = 0; i < glTF_data["animations"].size(); i++)
      {
        gltf::Animation animation;
        for (auto &x : glTF_data["animations"][i].items())
        {
          jsonConvertMacro(x, animation, name, std::string);
          if (std::string(x.key()) == "channels")
          {
            for (uint j = 0; j < x.value().size(); j++)
            {
              gltf::Animation::AnimationChannel channel;
              for (auto &y : x.value()[j].items())
              {
                jsonConvertMacro(y, channel, sampler, int);
                if (std::string(y.key()) == "target")
                {
                  for (auto &z : y.value().items())
                  {
                    jsonConvertMacro(z, channel.target, node, int);
                    if (std::string(z.key()) == "path")
                    {
                      if (z.value().get<std::string>() == "translation")
                      {
                        channel.target.path = gltf::Animation::AnimationChannel::AnimationTarget::translation;
                      }
                      else if (z.value().get<std::string>() == "rotation")
                      {
                        channel.target.path = gltf::Animation::AnimationChannel::AnimationTarget::rotation;
                      }
                      else if (z.value().get<std::string>() == "scale")
                      {
                        channel.target.path = gltf::Animation::AnimationChannel::AnimationTarget::scale;
                      }
                      else if (z.value().get<std::string>() == "weights")
                      {
                        channel.target.path = gltf::Animation::AnimationChannel::AnimationTarget::weights;
                      }
                      continue;
                    }
                    jsonExtensionMacro(z, channel.target, extensions);
                    jsonExtensionMacro(z, channel.target, extras);
                    jsonUnknownMacro(z, model.animations.channels.target);
                  }
                  continue;
                }
                jsonExtensionMacro(y, channel, extensions);
                jsonExtensionMacro(y, channel, extras);
                jsonUnknownMacro(y, model.animations.channels);
              }
              animation.channels.push_back(channel);
            }
            continue;
          }
          if (std::string(x.key()) == "samplers")
          {
            for (uint j = 0; j < x.value().size(); j++)
            {
              gltf::Animation::AnimationSampler sampler;
              for (auto &y : x.value()[j].items())
              {
                jsonConvertMacro(y, sampler, input, int);
                if (std::string(y.key()) == "interpolation")
                {
                  if (y.value().get<std::string>() == "LINEAR")
                  {
                    sampler.interpolation = gltf::Animation::AnimationSampler::LINEAR;
                  }
                  else if (y.value().get<std::string>() == "STEP")
                  {
                    sampler.interpolation = gltf::Animation::AnimationSampler::STEP;
                  }
                  else if (y.value().get<std::string>() == "CUBICSPLINE")
                  {
                    sampler.interpolation = gltf::Animation::AnimationSampler::CUBICSPLINE;
                  }
                  continue;
                }
                jsonConvertMacro(y, sampler, output, int);
                jsonExtensionMacro(y, sampler, extensions);
                jsonExtensionMacro(y, sampler, extras);
                jsonUnknownMacro(y, model.animations.samplers);
              }
              animation.samplers.push_back(sampler);
            }
            continue;
          }
          jsonExtensionMacro(x, animation, extensions);
          jsonExtensionMacro(x, animation, extras);
          jsonUnknownMacro(x, model.animations);
        }
        gltf.animations.push_back(animation);
      }
      continue;
    }
    jsonCondition(item, cameras)
    {
      for (uint i = 0; i < glTF_data["cameras"].size(); i++)
      {
        gltf::Camera camera;
        for (auto &x : glTF_data["cameras"][i].items())
        {
          jsonConvertMacro(x, camera, name, std::string);
          if (std::string(x.key()) == "orthographic")
          {
            for (auto &y : x.value().items())
            {
              jsonConvertMacro(y, camera.orthographic, xmag, float);
              jsonConvertMacro(y, camera.orthographic, ymag, float);
              jsonConvertMacro(y, camera.orthographic, zfar, float);
              jsonConvertMacro(y, camera.orthographic, znear, float);
              jsonExtensionMacro(y, camera.orthographic, extensions);
              jsonExtensionMacro(y, camera.orthographic, extras);
              jsonUnknownMacro(y, model.cameras.orthographic);
            }
            continue;
          }
          if (std::string(x.key()) == "perspective")
          {
            for (auto &y : x.value().items())
            {
              jsonConvertMacro(y, camera.perspective, aspectRatio, float);
              jsonConvertMacro(y, camera.perspective, yfov, float);
              jsonConvertMacro(y, camera.perspective, zfar, float);
              jsonConvertMacro(y, camera.perspective, znear, float);
              jsonExtensionMacro(y, camera.perspective, extensions);
              jsonExtensionMacro(y, camera.perspective, extras);
              jsonUnknownMacro(y, model.cameras.perspective);
            }
            continue;
          }
          if (std::string(x.key()) == "type")
          {
            if (x.value().get<std::string>() == "Perspective")
            {
              camera.type = gltf::Camera::Perspective;
            }
            else if (x.value().get<std::string>() == "Orthographic")
            {
              camera.type = gltf::Camera::Orthographic;
            }
            continue;
          }
          jsonExtensionMacro(x, camera, extensions);
          jsonExtensionMacro(x, camera, extras);
          jsonUnknownMacro(x, model.cameras);
        }
        gltf.cameras.push_back(camera);
      }
      continue;
    }
    jsonArrayMacro(item, gltf, extensionsUsed, std::string, i);
    jsonArrayMacro(item, gltf, extensionsRequired, std::string, i);
    jsonExtensionMacro(item, gltf, extensions);
    jsonExtensionMacro(item, gltf, extras);
  }
#undef jsonUnknownMacro
#undef jsonNodeArrayMacro
#undef jsonFunctionMacro
#undef jsonExtensionMacro
#undef jsonConvertMacro
#undef jsonConvertCastMacro
#undef jsonCondition

  // TODO(ANT) fix image name of older versions
  // for (uint i = 0; i < gltf.images.size(); i++)
  // {
  //   if (gltf.images[i].name.empty())
  //   {
  //     std::string extraName = glTF_data["images"][i]["extra"]["name"].get<std::string>();
  //     if (!extraName.empty())
  //     {
  //       gltf.images[i].name = extraName;
  //     }
  //   }
  // }

  // TODO(ANT) fix unique mesh names
  // too lazy to implement
  std::string *names = new std::string[gltf.meshes.size()];
  for (uint i = 0; i < gltf.meshes.size(); i++)
  {
    for (uint j = 0; j < i; j++)
    {
      if (gltf.meshes[i].name == names[j])
      {
        // TODO(ANT)
        // throw std::invalid_argument("duplicate name");
      }
    }
    names[i] = gltf.meshes[i].name;
  }
  delete[] names;
  return gltf;
}

namespace base64
{
  static unsigned int pos_of_char(const unsigned char chr)
  {
    //
    // Return the position of chr within base64_encode()
    //

    if (chr >= 'A' && chr <= 'Z')
      return chr - 'A';
    else if (chr >= 'a' && chr <= 'z')
      return chr - 'a' + ('Z' - 'A') + 1;
    else if (chr >= '0' && chr <= '9')
      return chr - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
    else if (chr == '+' || chr == '-')
      return 62; // Be liberal with input and accept both url ('-') and non-url ('+') base 64 characters (
    else if (chr == '/' || chr == '_')
      return 63; // Ditto for '/' and '_'
    else
      //
      // 2020-10-23: Throw std::exception rather than const char*
      //(Pablo Martin-Gomez, https://github.com/Bouska)
      //
      throw std::runtime_error("Input is not valid base64-encoded data.");
  }
  static std::string decode(std::string, bool);
  static std::string base64_decode(std::string const &s, bool remove_linebreaks = false)
  {
    return decode(s, remove_linebreaks);
  }
  static std::string decode(std::string encoded_string, bool remove_linebreaks = false)
  {
    //
    // decode(…) is templated so that it can be used with String = const std::string&
    // or std::string_view (requires at least C++17)
    //

    if (encoded_string.empty())
      return std::string();

    if (remove_linebreaks)
    {

      std::string copy(encoded_string);

      copy.erase(std::remove(copy.begin(), copy.end(), '\n'), copy.end());

      return base64_decode(copy, false);
    }

    size_t length_of_string = encoded_string.length();
    size_t pos = 0;

    //
    // The approximate length (bytes) of the decoded string might be one or
    // two bytes smaller, depending on the amount of trailing equal signs
    // in the encoded string. This approximation is needed to reserve
    // enough space in the string to be returned.
    //
    size_t approx_length_of_decoded_string = length_of_string / 4 * 3;
    std::string ret;
    ret.reserve(approx_length_of_decoded_string);

    while (pos < length_of_string)
    {
      //
      // Iterate over encoded input string in chunks. The size of all
      // chunks except the last one is 4 bytes.
      //
      // The last chunk might be padded with equal signs or dots
      // in order to make it 4 bytes in size as well, but this
      // is not required as per RFC 2045.
      //
      // All chunks except the last one produce three output bytes.
      //
      // The last chunk produces at least one and up to three bytes.
      //

      size_t pos_of_char_1 = pos_of_char(encoded_string[pos + 1]);

      //
      // Emit the first output byte that is produced in each chunk:
      //
      ret.push_back(static_cast<std::string::value_type>(((pos_of_char(encoded_string[pos + 0])) << 2) + ((pos_of_char_1 & 0x30) >> 4)));

      if ((pos + 2 < length_of_string) && // Check for data that is not padded with equal signs (which is allowed by RFC 2045)
          encoded_string[pos + 2] != '=' &&
          encoded_string[pos + 2] != '.' // accept URL-safe base 64 strings, too, so check for '.' also.
      )
      {
        //
        // Emit a chunk's second byte (which might not be produced in the last chunk).
        //
        unsigned int pos_of_char_2 = pos_of_char(encoded_string[pos + 2]);
        ret.push_back(static_cast<std::string::value_type>(((pos_of_char_1 & 0x0f) << 4) + ((pos_of_char_2 & 0x3c) >> 2)));

        if ((pos + 3 < length_of_string) &&
            encoded_string[pos + 3] != '=' &&
            encoded_string[pos + 3] != '.')
        {
          //
          // Emit a chunk's third byte (which might not be produced in the last chunk).
          //
          ret.push_back(static_cast<std::string::value_type>(((pos_of_char_2 & 0x03) << 6) + pos_of_char(encoded_string[pos + 3])));
        }
      }

      pos += 4;
    }

    return ret;
  }
}
static gltf::glTFModel parseGLTF(std::string path, BIN_t bin)
{
  gltf::glTFModel model;

  auto json = json::parse(bin.data, bin.data + bin.size);
  model = parseGLTFJSON(json);

  std::string dir = "./";
  {
    char sep = '/';
#ifdef _WIN32
    sep = '\\';
#endif
    size_t sepIndex = path.rfind(sep, path.length());
    if (sepIndex != std::string::npos)
    {
      dir = path.substr(0, sepIndex);
    }
  }

  for (uint i = 0; i < model.buffers.size(); i++)
  {
    if (model.buffers[i].uri.find("data:") == 0) // data URI
    {
      if (model.buffers[i].uri.find("base64") == std::string::npos)
      {
        assert(0 && "can only decode base64");
      }
      std::string data = model.buffers[i].uri.substr(model.buffers[i].uri.find(",") + 1);
      data = base64::decode(data);
      model.buffers[i].buffer = new uchar[data.length()];
      memcpy(model.buffers[i].buffer, data.data(), data.length());
    }
    else // file based
    {
      std::string file = dir + "/" + model.buffers[i].uri;
      std::ifstream input(file, std::ios::binary);
      std::vector<char> bytes(
          (std::istreambuf_iterator<char>(input)),
          (std::istreambuf_iterator<char>()));
      input.close();

      model.buffers[i].buffer = new uchar[bytes.size()];
      memcpy(model.buffers[i].buffer, bytes.data(), bytes.size());
    }
  }

  for (uint i = 0; i < model.images.size(); i++)
  {
  }

  return model;
}
static gltf::glTFModel parseGLB(std::string path, BIN_t bin)
{
  gltf::glTFModel model;
  membuild(
      struct {
        char glTF[4];
        uint32_t version;
        uint32_t dataSize;
      },
      header, pop_buff(bin.data, 12));
  printf("Loading model:%s\nglTF version %d size: %d, magic: %s\n", path.c_str(), header.version, header.dataSize, header.glTF);
  if (memcmp("glTF", header.glTF, 4))
  {
    fprintf(stderr, "Error loading file at " __FILE__ ":" toStr(__LINE__) ".\nUnexpected magic: %s. File might be corrupted\n", header.glTF);
    exit(1);
  }
  bin.size -= sizeof(header);

  struct chunk_t
  {
    uint32_t length;
    uint32_t type;
    uchar *data;
  };
#define JSON_GLB_CHUNK 0x4E4F534A
#define BIN_GLB_CHUNK 0x004E4942
  int dataSize = 12, nChunks = 0;
  chunk_t chunks[2];
  while (dataSize < header.dataSize)
  {
    chunk_t c = {CHVAL, CHVAL, 0};
    memcpy(&c, pop_buff(bin.data, 8), 8);
    assert(c.length != CHVAL && c.type != CHVAL);
    c.data = (uchar *)malloc(c.length);
    memcpy(c.data, pop_buff(bin.data, c.length), c.length);
    dataSize += 8 + c.length;
    if (c.type == JSON_GLB_CHUNK)
      chunks[0] = c;
    else if (c.type == BIN_GLB_CHUNK)
      chunks[1] = c;
    else
    {
      fprintf(stderr, "Error during loading file chunk %d at " __FILE__ ":" toStr(__LINE__) ".\nUnexpected chunk type:%X. File might be corrupted\n", nChunks, c.type);
      exit(1);
    }
  }

  json jsonData = json::parse(chunks[0].data, chunks[0].data + chunks[0].length);
  model = parseGLTFJSON(jsonData);

  if (model.buffers.size() != 1)
  {
    fprintf(stderr, "only 1 buffer should be present in file");
    exit(1);
  }
  model.buffers[0].buffer = (uchar *)malloc(chunks[1].length);
  memcpy(model.buffers[0].buffer, chunks[1].data, chunks[1].length);
  free(chunks[0].data);
  free(chunks[1].data);
  return model;
}

gltf::glTFModel loadModel(std::string path)
{
  Filetype filetype;
  gltf::glTFModel model;
  const char *dot = strrchr(path.c_str(), '.') + 1;
  if (!strcmp("glb", dot))
  {
    filetype = Filetype::glb;
  }
  else if (!strcmp("vrm", dot))
  {
    filetype = Filetype::vrm;
  }
  else if (!strcmp("gltf", dot))
  {
    filetype = Filetype::gltf;
  }
  else
  {
    filetype = Filetype::unknown;
  }

  BIN_t bin = {0, 0};
  bin.data = (uchar *)ch_bufferFile(path.c_str(), (void **)&bin.data, &bin.size);
  if (!bin.data)
  {
    fprintf(stderr, "Could not open file:%s\n", path.c_str());
    exit(1);
  }

  switch (filetype)
  {
  case Filetype::vrm:
  case Filetype::glb:
    model = parseGLB(path, bin);
    break;
  case Filetype::gltf:
    model = parseGLTF(path, bin);
    break;
  default:
    break;
  }
  free(bin.data);
  return model;
}
