#include "vanche_loader.hpp"
#include <chevan_utils_macro.h>
#include <nlohmann/json.hpp>
#include <chevan_utils_print.hpp>

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

#define jsonget(json, dst, name)                    \
  if (json.contains(#name))                         \
  {                                                 \
    dst.name = json[#name].get<typeof(dst.name)>(); \
  }
#define jsongetEnum(json, dst, name, loc, ...)               \
  if (json.contains(#name))                                  \
  {                                                          \
    std::string tmp = json[#name].get<std::string>();        \
    dst.name = CH_ENUM_PARSE(tmp.c_str(), loc, __VA_ARGS__); \
  }
#define jsongetEx(json, dst)                     \
  dst.extensions = deserialize_extensions(json); \
  dst.extras = deserialize_extras(json);

static ch_hash deserialize_extensions(json extensions);
static ch_hash deserialize_extras(json extensions);
static ch_hash deserialize_extensions(json extensions)
{
  ch_hash hash = {0};
  hash = ch_hashcreate(void *);
  if (extensions.contains("extensions"))
  {
    for (auto &ext : extensions["extensions"].items())
    {
      const char *key = ext.key().c_str();
      if (!strcmp(key, gltf::SUPPORTED_EXTENSIONS.KHR_materials_unlit))
      {
        gltf::Extensions::KHR_materials_unlit *unlit = new (malloc(sizeof(gltf::Extensions::KHR_materials_unlit))) gltf::Extensions::KHR_materials_unlit();
        unlit->extensions = deserialize_extensions(ext);
        unlit->extras = deserialize_extras(ext);
        ch_hashinsert(void *, hash, key, unlit);
      }
      else if (!strcmp(key, gltf::SUPPORTED_EXTENSIONS.KHR_texture_transform))
      {
        gltf::Extensions::KHR_texture_transform *transform = new (malloc(sizeof(gltf::Extensions::KHR_texture_transform))) gltf::Extensions::KHR_texture_transform();
        if (ext.value().contains("offset"))
        {
          std::vector<float> tmp = std::vector<float>();
          tmp = ext.value()["offset"].get<typeof(tmp)>();
          memcpy(transform->offset, tmp.data(), sizeof(transform->offset));
        }
        jsonget(ext.value(), (*transform), rotation);
        if (ext.value().contains("scale"))
        {
          std::vector<float> tmp = std::vector<float>();
          tmp = ext.value()["scale"].get<typeof(tmp)>();
          memcpy(transform->scale, tmp.data(), sizeof(transform->scale));
        }
        jsonget(ext.value(), (*transform), texCoord);
        jsongetEx(ext.value(), (*transform));
        ch_hashinsert(void *, hash, key, transform);
      }
      else if (!strcmp(key, gltf::SUPPORTED_EXTENSIONS.KHR_materials_emissive_strength))
      {
        gltf::Extensions::KHR_materials_emissive_strength *em = new (malloc(sizeof(gltf::Extensions::KHR_materials_emissive_strength))) gltf::Extensions::KHR_materials_emissive_strength();
        jsonget(ext.value(), (*em), emissiveStrength);
        jsongetEx(ext.value(), (*em));
        ch_hashinsert(void *, hash, key, em);
      }
      else if (!strcmp(key, gltf::SUPPORTED_EXTENSIONS.VRMC_springBone))
      {
        gltf::Extensions::VRMC_springBone *springBone = new (malloc(sizeof(gltf::Extensions::VRMC_springBone))) gltf::Extensions::VRMC_springBone();
        jsonget(ext.value(), (*springBone), specVersion);
        springBone->colliders = std::vector<gltf::Extensions::VRMC_springBone::Collider>();
        if (ext.value().contains("colliders"))
        {
          for (int i = 0; i < ext.value()["colliders"].size(); i++)
          {
            auto &y = ext.value()["colliders"][i];
            gltf::Extensions::VRMC_springBone::Collider coll;
            coll.node = y["node"].get<int>();
            if (y["shape"].contains("sphere"))
            {
              coll.shape.isSphere = 1;
              if (y["shape"]["sphere"].contains("offset"))
              {
                coll.shape.sphere.offset[0] = y["shape"]["sphere"]["offset"][0].get<float>();
                coll.shape.sphere.offset[1] = y["shape"]["sphere"]["offset"][1].get<float>();
                coll.shape.sphere.offset[2] = y["shape"]["sphere"]["offset"][2].get<float>();
              }
              if (y["shape"]["sphere"].contains("radius"))
                coll.shape.sphere.radius = y["shape"]["sphere"]["radius"].get<float>();
            }
            else
            {
              coll.shape.isSphere = 0;
              if (y["shape"]["capsule"].contains("offset"))
              {
                coll.shape.capsule.offset[0] = y["shape"]["capsule"]["offset"][0].get<float>();
                coll.shape.capsule.offset[1] = y["shape"]["capsule"]["offset"][1].get<float>();
                coll.shape.capsule.offset[2] = y["shape"]["capsule"]["offset"][2].get<float>();
              }
              if (y["shape"]["capsule"].contains("radius"))
                coll.shape.capsule.radius = y["shape"]["capsule"]["radius"].get<float>();
              if (y["shape"]["capsule"].contains("tail"))
              {
                coll.shape.capsule.tail[0] = y["shape"]["capsule"]["tail"][0].get<float>();
                coll.shape.capsule.tail[1] = y["shape"]["capsule"]["tail"][1].get<float>();
                coll.shape.capsule.tail[2] = y["shape"]["capsule"]["tail"][2].get<float>();
              }
            }
            springBone->colliders.push_back(coll);
          }
        }
        springBone->colliderGroups = std::vector<gltf::Extensions::VRMC_springBone::ColliderGroup>();
        if (ext.value().contains("colliderGroups"))
        {
          for (int i = 0; i < ext.value()["colliderGroups"].size(); i++)
          {
            auto &y = ext.value()["colliderGroups"][i];
            gltf::Extensions::VRMC_springBone::ColliderGroup collG;
            if (y.contains("name"))
              collG.name = y["name"].get<std::string>();
            collG.colliders = std::vector<int>();
            for (int j = 0; j < y["colliders"].size(); j++)
            {
              collG.colliders.push_back(y["colliders"][j].get<int>());
            }
            springBone->colliderGroups.push_back(collG);
          }
        }
        springBone->springs = std::vector<gltf::Extensions::VRMC_springBone::Spring>();
        if (ext.value().contains("springs"))
        {
          gltf::Extensions::VRMC_springBone::Spring spr;
          for (int i = 0; i < ext.value()["springs"].size(); i++)
          {
            auto &y = ext.value()["springs"][i];
            spr.joints = std::vector<gltf::Extensions::VRMC_springBone::Spring::Joint>();
            for (int i = 0; i < y["joints"].size(); i++)
            {
              gltf::Extensions::VRMC_springBone::Spring::Joint j;
              j.node = y["joints"][i]["node"].get<int>();
              if (y["joints"][i].contains("hitRadius"))
                j.hitRadius = y["joints"][i]["hitRadius"].get<float>();
              if (y["joints"][i].contains("stiffness"))
                j.stiffness = y["joints"][i]["stiffness"].get<float>();
              if (y["joints"][i].contains("gravityPower"))
                j.gravityPower = y["joints"][i]["gravityPower"].get<float>();
              if (y["joints"][i].contains("gravityDir"))
              {
                j.gravityDir[0] = y["joints"][i]["gravityDir"][0];
                j.gravityDir[1] = y["joints"][i]["gravityDir"][1];
                j.gravityDir[2] = y["joints"][i]["gravityDir"][2];
              }
              if (y["joints"][i].contains("dragForce"))
                j.dragForce = y["joints"][i]["dragForce"].get<float>();
              spr.joints.push_back(j);
            }
            if (y.contains("name"))
              spr.name = y["name"].get<std::string>();
            spr.colliderGroups = std::vector<int>();
            if (y.contains("colliderGroups"))
            {
              for (int i = 0; i < y["colliderGroups"].size(); i++)
              {
                spr.colliderGroups.push_back(y["colliderGroups"][i].get<int>());
              }
            }
            if (y.contains("center"))
              spr.center = y["center"].get<int>();
            springBone->springs.push_back(spr);
          }
        }

        ch_hashinsert(void *, hash, key, springBone);
      }
      else if (!strcmp(key, gltf::SUPPORTED_EXTENSIONS.VRMC_materials_mtoon))
      {
        gltf::Extensions::VRMC_materials_mtoon *mtoon = new (malloc(sizeof(gltf::Extensions::VRMC_materials_mtoon))) gltf::Extensions::VRMC_materials_mtoon();
        auto &item = ext.value();
        jsonget(item, (*mtoon), specVersion);
        jsonget(item, (*mtoon), transparentWithZWrite);
        jsonget(item, (*mtoon), renderQueueOffsetNumber);
        if (item.contains("shadeColorFactor"))
        {
          mtoon->shadeColorFactor[0] = item["shadeColorFactor"][0].get<float>();
          mtoon->shadeColorFactor[1] = item["shadeColorFactor"][1].get<float>();
          mtoon->shadeColorFactor[2] = item["shadeColorFactor"][2].get<float>();
        }
        if (item.contains("shadeMultiplyTexture"))
        {
          jsonget(item["shadeMultiplyTexture"], mtoon->shadeMultiplyTexture, index);
          jsonget(item["shadeMultiplyTexture"], mtoon->shadeMultiplyTexture, texCoord);
          jsongetEx(item["shadeMultiplyTexture"], mtoon->shadeMultiplyTexture);
        }
        jsonget(item, (*mtoon), shadingShiftFactor);
        if (item.contains("shadingShiftTexture"))
        {
          jsonget(item["shadingShiftTexture"], mtoon->shadingShiftTexture, index);
          jsonget(item["shadingShiftTexture"], mtoon->shadingShiftTexture, texCoord);
          jsonget(item["shadingShiftTexture"], mtoon->shadingShiftTexture, scale);
          jsongetEx(item["shadingShiftTexture"], mtoon->shadingShiftTexture);
        }
        jsonget(item, (*mtoon), shadingToonyFactor);
        jsonget(item, (*mtoon), giEqualizationFactor);
        if (item.contains("matcapFactor"))
        {
          mtoon->matcapFactor[0] = item["matcapFactor"][0].get<float>();
          mtoon->matcapFactor[1] = item["matcapFactor"][1].get<float>();
          mtoon->matcapFactor[2] = item["matcapFactor"][2].get<float>();
        }
        if (item.contains("matcapTexture"))
        {
          jsonget(item["matcapTexture"], mtoon->matcapTexture, index);
          jsonget(item["matcapTexture"], mtoon->matcapTexture, texCoord);
          jsongetEx(item["matcapTexture"], mtoon->matcapTexture);
        }
        if (item.contains("parametricRimColorFactor"))
        {
          mtoon->parametricRimColorFactor[0] = item["parametricRimColorFactor"][0].get<float>();
          mtoon->parametricRimColorFactor[1] = item["parametricRimColorFactor"][1].get<float>();
          mtoon->parametricRimColorFactor[2] = item["parametricRimColorFactor"][2].get<float>();
        }
        if (item.contains("rimMultiplyTexture"))
        {
          jsonget(item["rimMultiplyTexture"], mtoon->rimMultiplyTexture, index);
          jsonget(item["rimMultiplyTexture"], mtoon->rimMultiplyTexture, texCoord);
          if (item["rimMultiplyTexture"].contains("extensions"))
          {
            mtoon->rimMultiplyTexture.extensions = deserialize_extensions(item["rimMultiplyTexture"]["extensions"]);
          }
          if (item["rimMultiplyTexture"].contains("extras"))
          {
            mtoon->rimMultiplyTexture.extras = deserialize_extras(item["rimMultiplyTexture"]["extras"]);
          }
        }
        jsonget(item, (*mtoon), rimLightingMixFactor);
        jsonget(item, (*mtoon), parametricRimFresnelPowerFactor);
        jsonget(item, (*mtoon), parametricRimLiftFactor);
        if (item.contains("outlineWidthMode"))
        {
          mtoon->outlineWidthMode = (item["outlineWidthMode"].get<std::string>() == "worldCoordinates" ? gltf::Extensions::VRMC_materials_mtoon::OutlineWidthMode::worldCoordinates : (item["outlineWidthMode"].get<std::string>() == "screenCoordinates" ? gltf::Extensions::VRMC_materials_mtoon::OutlineWidthMode::screenCoordinates : gltf::Extensions::VRMC_materials_mtoon::OutlineWidthMode::none));
        }
        jsonget(item, (*mtoon), outlineWidthFactor);
        if (item.contains("outlineWidthMultiplyTexture"))
        {
          jsonget(item["outlineWidthMultiplyTexture"], mtoon->outlineWidthMultiplyTexture, index);
          jsonget(item["outlineWidthMultiplyTexture"], mtoon->outlineWidthMultiplyTexture, texCoord);
          if (item["outlineWidthMultiplyTexture"].contains("extensions"))
          {
            mtoon->outlineWidthMultiplyTexture.extensions = deserialize_extensions(item["outlineWidthMultiplyTexture"]["extensions"]);
          }
          if (item["outlineWidthMultiplyTexture"].contains("extras"))
          {
            mtoon->outlineWidthMultiplyTexture.extras = deserialize_extras(item["outlineWidthMultiplyTexture"]["extras"]);
          }
        }
        if (item.contains("outlineColorFactor"))
        {
          mtoon->outlineColorFactor[0] = item["outlineColorFactor"][0].get<float>();
          mtoon->outlineColorFactor[1] = item["outlineColorFactor"][1].get<float>();
          mtoon->outlineColorFactor[2] = item["outlineColorFactor"][2].get<float>();
        }
        jsonget(item, (*mtoon), outlineLightingMixFactor);
        if (item.contains("uvAnimationMaskTexture"))
        {
          jsonget(item["uvAnimationMaskTexture"], mtoon->uvAnimationMaskTexture, index);
          jsonget(item["uvAnimationMaskTexture"], mtoon->uvAnimationMaskTexture, texCoord);
          if (item["uvAnimationMaskTexture"].contains("extensions"))
          {
            mtoon->uvAnimationMaskTexture.extensions = deserialize_extensions(item["uvAnimationMaskTexture"]["extensions"]);
          }
          if (item["uvAnimationMaskTexture"].contains("extras"))
          {
            mtoon->uvAnimationMaskTexture.extras = deserialize_extras(item["uvAnimationMaskTexture"]["extras"]);
          }
        }
        jsonget(item, (*mtoon), uvAnimationScrollXSpeedFactor);
        jsonget(item, (*mtoon), uvAnimationScrollYSpeedFactor);
        jsonget(item, (*mtoon), uvAnimationRotationSpeedFactor);
        ch_hashinsert(void *, hash, key, mtoon);
      }
      else if (!strcmp(key, gltf::SUPPORTED_EXTENSIONS.VRMC_node_constraint))
      {
        gltf::Extensions::VRMC_node_constraint *node = new (malloc(sizeof(gltf::Extensions::VRMC_node_constraint))) gltf::Extensions::VRMC_node_constraint();
        auto &item = ext.value();
        jsonget(item, (*node), specVersion);
        if (item.contains("specVersion"))
        {
          std::string tmp = item["specVersion"].get<std::string>();
          (*node).specVersion = item["specVersion"].get<std::string>();
        }

        if (item["constraint"].contains("roll"))
        {
          node->constraint.roll.source = item["constraint"]["roll"]["source"].get<int>();
          std::string rollAxis = item["constraint"]["roll"]["rollAxis"].get<std::string>();
          node->constraint.roll.rollAxis = (rollAxis == "X" ? node->constraint.roll.X : (rollAxis == "Y" ? node->constraint.roll.Y : node->constraint.roll.Z));
          jsonget(item["constraint"]["roll"], node->constraint.roll, weight);
        }
        else if (item["constraint"].contains("aim"))
        {
          node->constraint.aim.source = item["constraint"]["aim"]["source"].get<int>();
          std::string aimAxis = item["constraint"]["aim"]["aimAxis"].get<std::string>();
          if (aimAxis == "PositiveX")
          {
            node->constraint.aim.aimAxis = node->constraint.aim.PositiveX;
          }
          else if (aimAxis == "NegativeX")
          {
            node->constraint.aim.aimAxis = node->constraint.aim.NegativeX;
          }
          else if (aimAxis == "PositiveY")
          {
            node->constraint.aim.aimAxis = node->constraint.aim.PositiveY;
          }
          else if (aimAxis == "NegativeY")
          {
            node->constraint.aim.aimAxis = node->constraint.aim.NegativeY;
          }
          else if (aimAxis == "PositiveZ")
          {
            node->constraint.aim.aimAxis = node->constraint.aim.PositiveZ;
          }
          else if (aimAxis == "NegativeZ")
          {
            node->constraint.aim.aimAxis = node->constraint.aim.NegativeZ;
          }
          jsonget(item["constraint"]["aim"], node->constraint.aim, weight);
        }
        else if (item["constraint"].contains("rotation"))
        {
          node->constraint.rotation.source = item["constraint"]["rotation"]["source"].get<int>();
          jsonget(item["constraint"]["rotation"], node->constraint.rotation, weight);
        }
        ch_hashinsert(void *, hash, key, node);
      }
      else if (!strcmp(key, gltf::SUPPORTED_EXTENSIONS.VRMC_vrm))
      {
        gltf::Extensions::VRMC_vrm *vrm = new (malloc(sizeof(gltf::Extensions::VRMC_vrm))) gltf::Extensions::VRMC_vrm();
        auto &item = ext.value();
        jsonget(item, (*vrm), specVersion);
        jsonget(item["meta"], vrm->meta, name);
        jsonget(item["meta"], vrm->meta, version);
        jsonget(item["meta"], vrm->meta, authors);
        jsonget(item["meta"], vrm->meta, copyrightInformation);
        jsonget(item["meta"], vrm->meta, contactInformation);
        jsonget(item["meta"], vrm->meta, reference);
        jsonget(item["meta"], vrm->meta, thirdPartyLicenses);
        jsonget(item["meta"], vrm->meta, licenseUrl);
        jsongetEnum(item["meta"], vrm->meta, avatarPermission, vrm->meta., avatarPermission, onlySeparatelyLicensedPerson, everyone);
        jsonget(item["meta"], vrm->meta, allowExcessivelyViolentUsage);
        jsonget(item["meta"], vrm->meta, allowExcessivelySexualUsage);
        jsongetEnum(item["meta"], vrm->meta, commercialUsage, vrm->meta., personalNonProfit, personalProfit, corporation);
        jsonget(item["meta"], vrm->meta, allowPoliticalOrReligiousUsage);
        jsonget(item["meta"], vrm->meta, allowAntisocialOrHateUsage);
        jsongetEnum(item["meta"], vrm->meta, creditNotation, vrm->meta., required, unnecessary);
        jsonget(item["meta"], vrm->meta, allowRedistribution);
        jsongetEnum(item["meta"], vrm->meta, modification, vrm->meta., prohibited, allowModification, allowModificationRedistribution);
        jsonget(item["meta"], vrm->meta, otherLicenseUrl);
        if (item.contains("humanoid"))
        {
#define HumanBone(NAME)                                                                            \
  if (item["humanoid"]["humanBones"].contains(#NAME))                                              \
  {                                                                                                \
    vrm->humanoid.humanBones.NAME.node = item["humanoid"]["humanBones"][#NAME]["node"].get<int>(); \
  }
          HumanBone(hips);
          HumanBone(spine);
          HumanBone(chest);
          HumanBone(upperChest);
          HumanBone(neck);
          HumanBone(head);
          HumanBone(leftEye);
          HumanBone(rightEye);
          HumanBone(jaw);
          HumanBone(leftUpperLeg);
          HumanBone(leftLowerLeg);
          HumanBone(leftFoot);
          HumanBone(leftToes);
          HumanBone(rightUpperLeg);
          HumanBone(rightLowerLeg);
          HumanBone(rightFoot);
          HumanBone(rightToes);
          HumanBone(leftShoulder);
          HumanBone(leftUpperArm);
          HumanBone(leftLowerArm);
          HumanBone(leftHand);
          HumanBone(rightShoulder);
          HumanBone(rightUpperArm);
          HumanBone(rightLowerArm);
          HumanBone(rightHand);
          HumanBone(leftThumbMetacarpal);
          HumanBone(leftThumbProximal);
          HumanBone(leftThumbDistal);
          HumanBone(leftIndexProximal);
          HumanBone(leftIndexIntermediate);
          HumanBone(leftIndexDistal);
          HumanBone(leftMiddleProximal);
          HumanBone(leftMiddleIntermediate);
          HumanBone(leftMiddleDistal);
          HumanBone(leftRingProximal);
          HumanBone(leftRingIntermediate);
          HumanBone(leftRingDistal);
          HumanBone(leftLittleProximal);
          HumanBone(leftLittleIntermediate);
          HumanBone(leftLittleDistal);
          HumanBone(rightThumbMetacarpal);
          HumanBone(rightThumbProximal);
          HumanBone(rightThumbDistal);
          HumanBone(rightIndexProximal);
          HumanBone(rightIndexIntermediate);
          HumanBone(rightIndexDistal);
          HumanBone(rightMiddleProximal);
          HumanBone(rightMiddleIntermediate);
          HumanBone(rightMiddleDistal);
          HumanBone(rightRingProximal);
          HumanBone(rightRingIntermediate);
          HumanBone(rightRingDistal);
          HumanBone(rightLittleProximal);
          HumanBone(rightLittleIntermediate);
          HumanBone(rightLittleDistal);
#undef HumanBone
        }
        if (item.contains("firstPerson"))
        {
          if (item["firstPerson"].contains("meshAnnotations"))
          {
            vrm->firstPerson.meshAnnotations = std::vector<gltf::Extensions::VRMC_vrm::FirstPerson::MeshAnnotations>();
            for (int i = 0; i < item["firstPerson"]["meshAnnotations"].size(); i++)
            {
              gltf::Extensions::VRMC_vrm::FirstPerson::MeshAnnotations ma;
              ma.node = item["firstPerson"]["meshAnnotations"][i]["node"].get<int>();
              std::string str = item["firstPerson"]["meshAnnotations"][i]["type"].get<std::string>();
              if (str == "auto")
              {
                ma.type = ma.Auto;
              }
              else if (str == "both")
              {
                ma.type = ma.both;
              }
              else if (str == "thirdPersonOnly")
              {
                ma.type = ma.thirdPersonOnly;
              }
              else if (str == "firstPersonOnly")
              {
                ma.type = ma.firstPersonOnly;
              }
              vrm->firstPerson.meshAnnotations.push_back(ma);
            }
          }
        }
        if (item.contains("lookAt"))
        {
          if (item["lookAt"].contains("offsetFromHeadBone"))
          {
            vrm->lookAt.offsetFromHeadBone[0] = item["lookAt"]["offsetFromHeadBone"][0].get<float>();
            vrm->lookAt.offsetFromHeadBone[1] = item["lookAt"]["offsetFromHeadBone"][1].get<float>();
            vrm->lookAt.offsetFromHeadBone[2] = item["lookAt"]["offsetFromHeadBone"][2].get<float>();
          }
          if (item["lookAt"].contains("type"))
          {
            std::string str = item["lookAt"]["type"].get<std::string>();
            if (str == "bone")
            {
              vrm->lookAt.type = vrm->lookAt.bone;
            }
            else if (str == "expression")
            {
              vrm->lookAt.type = vrm->lookAt.expression;
            }
          }
          if (item["lookAt"].contains("rangeMapHorizontalInner"))
          {
            jsonget(item["lookAt"]["rangeMapHorizontalInner"], vrm->lookAt.rangeMapHorizontalInner, inputMaxValue);
            jsonget(item["lookAt"]["rangeMapHorizontalInner"], vrm->lookAt.rangeMapHorizontalInner, outputScale);
          }
          if (item["lookAt"].contains("rangeMapHorizontalOuter"))
          {
            jsonget(item["lookAt"]["rangeMapHorizontalOuter"], vrm->lookAt.rangeMapHorizontalOuter, inputMaxValue);
            jsonget(item["lookAt"]["rangeMapHorizontalOuter"], vrm->lookAt.rangeMapHorizontalOuter, outputScale);
          }
          if (item["lookAt"].contains("rangeMapVerticalDown"))
          {
            jsonget(item["lookAt"]["rangeMapVerticalDown"], vrm->lookAt.rangeMapVerticalDown, inputMaxValue);
            jsonget(item["lookAt"]["rangeMapVerticalDown"], vrm->lookAt.rangeMapVerticalDown, outputScale);
          }
          if (item["lookAt"].contains("rangeMapVerticalUp"))
          {
            jsonget(item["lookAt"]["rangeMapVerticalUp"], vrm->lookAt.rangeMapVerticalUp, inputMaxValue);
            jsonget(item["lookAt"]["rangeMapVerticalUp"], vrm->lookAt.rangeMapVerticalUp, outputScale);
          }
        }
        if (item.contains("expressions"))
        {
          if (item["expressions"].contains("preset"))
          {
#define ExpressionParse(EXP)                                                                                                                                                                                                                                                                       \
  if (item["expressions"]["preset"].contains(#EXP))                                                                                                                                                                                                                                                \
  {                                                                                                                                                                                                                                                                                                \
    if (item["expressions"]["preset"][#EXP].contains("morphTargetBinds"))                                                                                                                                                                                                                          \
    {                                                                                                                                                                                                                                                                                              \
      vrm->expressions.preset.EXP.morphTargetBinds =                                                                                                                                                                                                                                               \
          std::vector<gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::MorphTargetBind>(item["expressions"]["preset"][#EXP]["morphTargetBinds"].size());                                                                                                                                 \
      for (int i = 0; i < item["expressions"]["preset"][#EXP]["morphTargetBinds"].size(); i++)                                                                                                                                                                                                     \
      {                                                                                                                                                                                                                                                                                            \
        vrm->expressions.preset.EXP.morphTargetBinds[i].node = item["expressions"]["preset"][#EXP]["morphTargetBinds"][i]["node"].get<int>();                                                                                                                                                      \
        vrm->expressions.preset.EXP.morphTargetBinds[i].index = item["expressions"]["preset"][#EXP]["morphTargetBinds"][i]["index"].get<int>();                                                                                                                                                    \
        vrm->expressions.preset.EXP.morphTargetBinds[i].weight = item["expressions"]["preset"][#EXP]["morphTargetBinds"][i]["weight"].get<float>();                                                                                                                                                \
      }                                                                                                                                                                                                                                                                                            \
    }                                                                                                                                                                                                                                                                                              \
    if (item["expressions"]["preset"][#EXP].contains("materialColorBinds"))                                                                                                                                                                                                                        \
    {                                                                                                                                                                                                                                                                                              \
      vrm->expressions.preset.EXP.materialColorBinds =                                                                                                                                                                                                                                             \
          std::vector<gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::MaterialColorBind>(item["expressions"]["preset"][#EXP]["materialColorBinds"].size());                                                                                                                             \
                                                                                                                                                                                                                                                                                                   \
      for (int i = 0; i < item["expressions"]["preset"][#EXP]["materialColorBinds"].size(); i++)                                                                                                                                                                                                   \
      {                                                                                                                                                                                                                                                                                            \
        vrm->expressions.preset.EXP.materialColorBinds[i].material = item["expressions"]["preset"][#EXP]["materialColorBinds"][i]["material"].get<int>();                                                                                                                                          \
        jsongetEnum(item["expressions"]["preset"][#EXP]["materialColorBinds"][i], vrm->expressions.preset.EXP.materialColorBinds[i], type, gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::MaterialColorBind::, color, emissionColor, shadeColor, matcapColor, rimColor, outlineColor); \
        vrm->expressions.preset.EXP.materialColorBinds[i].targetValue[0] = item["expressions"]["preset"][#EXP]["materialColorBinds"][i]["targetValue"][0].get<float>();                                                                                                                            \
        vrm->expressions.preset.EXP.materialColorBinds[i].targetValue[1] = item["expressions"]["preset"][#EXP]["materialColorBinds"][i]["targetValue"][1].get<float>();                                                                                                                            \
        vrm->expressions.preset.EXP.materialColorBinds[i].targetValue[2] = item["expressions"]["preset"][#EXP]["materialColorBinds"][i]["targetValue"][2].get<float>();                                                                                                                            \
        vrm->expressions.preset.EXP.materialColorBinds[i].targetValue[3] = item["expressions"]["preset"][#EXP]["materialColorBinds"][i]["targetValue"][3].get<float>();                                                                                                                            \
      }                                                                                                                                                                                                                                                                                            \
    }                                                                                                                                                                                                                                                                                              \
    if (item["expressions"]["preset"][#EXP].contains("textureTransformBinds"))                                                                                                                                                                                                                     \
    {                                                                                                                                                                                                                                                                                              \
      vrm->expressions.preset.EXP.textureTransformBinds =                                                                                                                                                                                                                                          \
          std::vector<gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::TextureTransformBind>(item["expressions"]["preset"][#EXP]["textureTransformBinds"].size());                                                                                                                       \
      for (int i = 0; i < item["expressions"]["preset"][#EXP]["textureTransformBinds"].size(); i++)                                                                                                                                                                                                \
      {                                                                                                                                                                                                                                                                                            \
        vrm->expressions.preset.EXP.textureTransformBinds[i].material = item["expressions"]["preset"][#EXP]["textureTransformBinds"][i]["material"].get<int>();                                                                                                                                    \
        if (item["expressions"]["preset"][#EXP]["textureTransformBinds"][i].contains("scale"))                                                                                                                                                                                                     \
        {                                                                                                                                                                                                                                                                                          \
          vrm->expressions.preset.EXP.textureTransformBinds[i].scale[0] = item["expressions"]["preset"][#EXP]["textureTransformBinds"][i]["scale"][0].get<float>();                                                                                                                                \
          vrm->expressions.preset.EXP.textureTransformBinds[i].scale[1] = item["expressions"]["preset"][#EXP]["textureTransformBinds"][i]["scale"][1].get<float>();                                                                                                                                \
        }                                                                                                                                                                                                                                                                                          \
        if (item["expressions"]["preset"][#EXP]["textureTransformBinds"][i].contains("offset"))                                                                                                                                                                                                    \
        {                                                                                                                                                                                                                                                                                          \
          vrm->expressions.preset.EXP.textureTransformBinds[i].offset[0] = item["expressions"]["preset"][#EXP]["textureTransformBinds"][i]["offset"][0].get<float>();                                                                                                                              \
          vrm->expressions.preset.EXP.textureTransformBinds[i].offset[1] = item["expressions"]["preset"][#EXP]["textureTransformBinds"][i]["offset"][1].get<float>();                                                                                                                              \
        }                                                                                                                                                                                                                                                                                          \
      }                                                                                                                                                                                                                                                                                            \
    }                                                                                                                                                                                                                                                                                              \
    jsonget(item["expressions"]["preset"][#EXP], vrm->expressions.preset.EXP, isBinary);                                                                                                                                                                                                           \
    jsongetEnum(item["expressions"]["preset"][#EXP], vrm->expressions.preset.EXP, overrideBlink, gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::, none, block, blend);                                                                                                                 \
    jsongetEnum(item["expressions"]["preset"][#EXP], vrm->expressions.preset.EXP, overrideLookAt, gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::, none, block, blend);                                                                                                                \
    jsongetEnum(item["expressions"]["preset"][#EXP], vrm->expressions.preset.EXP, overrideMouth, gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::, none, block, blend);                                                                                                                 \
  }
            ExpressionParse(happy);
            ExpressionParse(angry);
            ExpressionParse(sad);
            ExpressionParse(relaxed);
            ExpressionParse(surprised);
            ExpressionParse(aa);
            ExpressionParse(ih);
            ExpressionParse(ou);
            ExpressionParse(ee);
            ExpressionParse(oh);
            ExpressionParse(blink);
            ExpressionParse(blinkLeft);
            ExpressionParse(blinkRight);
            ExpressionParse(lookUp);
            ExpressionParse(lookDown);
            ExpressionParse(lookLeft);
            ExpressionParse(lookRight);
            ExpressionParse(neutral);

#undef ExpressionParse
          }
        }
        ch_hashinsert(void *, hash, key, vrm);
      }
      else
      {
        chprinterr("%s: extension not supported\n", ext.key().c_str());
        continue;
      }
    }
  }
  return hash;
}
static ch_hash deserialize_extras(json extras)
{
  ch_hash hash = {0};
  hash = ch_hashcreate(void *);
  if (extras.contains("extras"))
  {
    for (auto &ext : extras["extras"].items())
    {
      const char *key = ext.key().c_str();
      if (!strcmp(key, gltf::SUPPORTED_EXTRAS.TargetNames))
      {
        gltf::Extras::TargetNames *unlit = new (malloc(sizeof(gltf::Extras::TargetNames))) gltf::Extras::TargetNames();
        ch_hashget(void *, hash, key) = unlit;
      }
      else
      {
        chprinterr("%s: extra not supported\n", ext.key().c_str());
        continue;
      }
    }
  }
  return hash;
}

static gltf::glTFModel parseGLTFJSON(json data)
{
  gltf::glTFModel gltf;
  if (data.contains("asset"))
  {
    jsonget(data["asset"], gltf.asset, version);
    jsonget(data["asset"], gltf.asset, generator);
    jsonget(data["asset"], gltf.asset, copyright);
    jsonget(data["asset"], gltf.asset, minVersion);
    gltf.asset.extensions = deserialize_extensions(data["asset"]);
    gltf.asset.extras = deserialize_extras(data["asset"]);
  }
  if (data.contains("buffers"))
  {
    for (int i = 0; i < data["buffers"].size(); i++)
    {
      gltf::Buffer b;
      jsonget(data["buffers"][i], b, uri);
      jsonget(data["buffers"][i], b, byteLength);
      jsonget(data["buffers"][i], b, name);
      b.extensions = deserialize_extensions(data["buffers"][i]);
      b.extras = deserialize_extras(data["buffers"][i]);
      gltf.buffers.push_back(b);
    }
  }
  if (data.contains("bufferViews"))
  {
    for (int i = 0; i < data["bufferViews"].size(); i++)
    {
      gltf::BufferView bv;
      jsonget(data["bufferViews"][i], bv, buffer);
      jsonget(data["bufferViews"][i], bv, byteOffset);
      jsonget(data["bufferViews"][i], bv, byteLength);
      jsonget(data["bufferViews"][i], bv, byteStride);
      jsonget(data["bufferViews"][i], bv, target);
      jsonget(data["bufferViews"][i], bv, name);
      bv.extensions = deserialize_extensions(data["bufferViews"][i]);
      bv.extras = deserialize_extras(data["bufferViews"][i]);
      gltf.bufferViews.push_back(bv);
    }
  }
  if (data.contains("accessors"))
  {
    for (int i = 0; i < data["accessors"].size(); i++)
    {
      gltf::Accessor ac;
      jsonget(data["accessors"][i], ac, bufferView);
      jsonget(data["accessors"][i], ac, byteOffset);
      if (data["accessors"][i].contains("type"))
      {
        ac.type = CH_ENUM_PARSE(data["accessors"][i]["type"].get<std::string>().c_str(), gltf::Accessor::Types::, SCALAR, VEC2, VEC3, VEC4, MAT2, MAT3, MAT4);
      }
      jsonget(data["accessors"][i], ac, componentType);
      jsonget(data["accessors"][i], ac, count);
      jsonget(data["accessors"][i], ac, max);
      jsonget(data["accessors"][i], ac, min);
      jsonget(data["accessors"][i], ac, normalized);
      if (data["accessors"][i].contains("sparse"))
      {
        jsonget(data["accessors"][i]["sparse"], ac.sparse, count);
        if (data["accessors"][i]["sparse"].contains("indices"))
        {
          jsonget(data["accessors"][i]["sparse"]["indices"], ac.sparse.indices, bufferView);
          jsonget(data["accessors"][i]["sparse"]["indices"], ac.sparse.indices, byteOffset);
          jsonget(data["accessors"][i]["sparse"]["indices"], ac.sparse.indices, componentType);
          ac.sparse.indices.extensions = deserialize_extensions(data["accessors"][i]["sparse"]["indices"]);
          ac.sparse.indices.extensions = deserialize_extras(data["accessors"][i]["sparse"]["indices"]);
        }
        if (data["accessors"][i]["sparse"].contains("values"))
        {
          jsonget(data["accessors"][i]["sparse"]["values"], ac.sparse.values, bufferView);
          jsonget(data["accessors"][i]["sparse"]["values"], ac.sparse.values, byteOffset);
          ac.sparse.values.extensions = deserialize_extensions(data["accessors"][i]["sparse"]["values"]);
          ac.sparse.values.extensions = deserialize_extras(data["accessors"][i]["sparse"]["values"]);
        }
        ac.sparse.extensions = deserialize_extensions(data["accessors"][i]["sparse"]);
        ac.sparse.extras = deserialize_extras(data["accessors"][i]["sparse"]);
      }
      jsonget(data["accessors"][i], ac, name);
      ac.extensions = deserialize_extensions(data["accessors"][i]);
      ac.extras = deserialize_extras(data["accessors"][i]);
      gltf.accessors.push_back(ac);
    }
  }
  if (data.contains("textures"))
  {
    for (int i = 0; i < data["textures"].size(); i++)
    {
      gltf::Texture tex;
      jsonget(data["textures"][i], tex, sampler);
      jsonget(data["textures"][i], tex, source);
      jsonget(data["textures"][i], tex, name);
      tex.extensions = deserialize_extensions(data["textures"][i]);
      tex.extras = deserialize_extras(data["textures"][i]);
      gltf.textures.push_back(tex);
    }
  }
  if (data.contains("samplers"))
  {
    for (int i = 0; i < data["samplers"].size(); i++)
    {
      gltf::Sampler s;
      jsonget(data["samplers"][i], s, magFilter);
      jsonget(data["samplers"][i], s, minFilter);
      jsonget(data["samplers"][i], s, wrapS);
      jsonget(data["samplers"][i], s, wrapT);
      jsonget(data["samplers"][i], s, name);
      s.extensions = deserialize_extensions(data["samplers"][i]);
      s.extras = deserialize_extras(data["samplers"][i]);
      gltf.samplers.push_back(s);
    }
  }
  if (data.contains("images"))
  {
    for (int i = 0; i < data["images"].size(); i++)
    {
      gltf::Image im;
      jsonget(data["images"][i], im, name);
      jsonget(data["images"][i], im, uri);
      jsonget(data["images"][i], im, bufferView);
      jsonget(data["images"][i], im, mimeType);
      im.extensions = deserialize_extensions(data["images"][i]);
      im.extras = deserialize_extras(data["images"][i]);
      gltf.images.push_back(im);
    }
  }
  if (data.contains("materials"))
  {
    for (int i = 0; i < data["materials"].size(); i++)
    {
      gltf::Material mat;
      jsonget(data["materials"][i], mat, name);
      if (data["materials"][i].contains("pbrMetallicRoughness"))
      {
        if (data["materials"][i]["pbrMetallicRoughness"].contains("baseColorTexture"))
        {
          jsonget(data["materials"][i]["pbrMetallicRoughness"]["baseColorTexture"], mat.pbrMetallicRoughness.baseColorTexture, index);
          jsonget(data["materials"][i]["pbrMetallicRoughness"]["baseColorTexture"], mat.pbrMetallicRoughness.baseColorTexture, texCoord);
          mat.pbrMetallicRoughness.baseColorTexture.extensions = deserialize_extensions(data["materials"][i]["pbrMetallicRoughness"]["baseColorTexture"]);
          mat.pbrMetallicRoughness.baseColorTexture.extras = deserialize_extras(data["materials"][i]["pbrMetallicRoughness"]["baseColorTexture"]);
        }
        jsonget(data["materials"][i]["pbrMetallicRoughness"], mat.pbrMetallicRoughness, baseColorFactor);
        if (data["materials"][i]["pbrMetallicRoughness"].contains("metallicRoughnessTexture"))
        {
          jsonget(data["materials"][i]["pbrMetallicRoughness"]["metallicRoughnessTexture"], mat.pbrMetallicRoughness.metallicRoughnessTexture, index);
          jsonget(data["materials"][i]["pbrMetallicRoughness"]["metallicRoughnessTexture"], mat.pbrMetallicRoughness.metallicRoughnessTexture, texCoord);
          mat.pbrMetallicRoughness.metallicRoughnessTexture.extensions = deserialize_extensions(data["materials"][i]["pbrMetallicRoughness"]["metallicRoughnessTexture"]);
          mat.pbrMetallicRoughness.metallicRoughnessTexture.extras = deserialize_extras(data["materials"][i]["pbrMetallicRoughness"]["metallicRoughnessTexture"]);
        }
        jsonget(data["materials"][i]["pbrMetallicRoughness"], mat.pbrMetallicRoughness, metallicFactor);
        jsonget(data["materials"][i]["pbrMetallicRoughness"], mat.pbrMetallicRoughness, roughnessFactor);
        mat.pbrMetallicRoughness.extensions = deserialize_extensions(data["materials"][i]["pbrMetallicRoughness"]);
        mat.pbrMetallicRoughness.extras = deserialize_extras(data["materials"][i]["pbrMetallicRoughness"]);
      }
      if (data["materials"][i].contains("normalTexture"))
      {
        jsonget(data["materials"][i]["normalTexture"], mat.normalTexture, index);
        jsonget(data["materials"][i]["normalTexture"], mat.normalTexture, texCoord);
        jsonget(data["materials"][i]["normalTexture"], mat.normalTexture, scale);
        mat.normalTexture.extensions = deserialize_extensions(data["materials"][i]["normalTexture"]);
        mat.normalTexture.extras = deserialize_extras(data["materials"][i]["normalTexture"]);
      }
      if (data["materials"][i].contains("occlusionTexture"))
      {
        jsonget(data["materials"][i]["occlusionTexture"], mat.occlusionTexture, index);
        jsonget(data["materials"][i]["occlusionTexture"], mat.occlusionTexture, texCoord);
        jsonget(data["materials"][i]["occlusionTexture"], mat.occlusionTexture, strength);
        mat.occlusionTexture.extensions = deserialize_extensions(data["materials"][i]["occlusionTexture"]);
        mat.occlusionTexture.extras = deserialize_extras(data["materials"][i]["occlusionTexture"]);
      }
      if (data["materials"][i].contains("emissiveTexture"))
      {
        jsonget(data["materials"][i]["emissiveTexture"], mat.emissiveTexture, index);
        jsonget(data["materials"][i]["emissiveTexture"], mat.emissiveTexture, texCoord);
        mat.emissiveTexture.extensions = deserialize_extensions(data["materials"][i]["emissiveTexture"]);
        mat.emissiveTexture.extras = deserialize_extras(data["materials"][i]["emissiveTexture"]);
      }
      jsonget(data["materials"][i], mat, emissiveFactor);
      jsongetEnum(data["materials"][i], mat, alphaMode, gltf::Material::AlphaMode::, OPAQUE, MASK, BLEND);
      jsonget(data["materials"][i], mat, alphaCutoff);
      jsonget(data["materials"][i], mat, doubleSided);
      mat.extensions = deserialize_extensions(data["materials"][i]);
      mat.extras = deserialize_extras(data["materials"][i]);
      gltf.materials.push_back(mat);
    }
  }
  if (data.contains("meshes"))
  {
    for (int i = 0; i < data["meshes"].size(); i++)
    {
      gltf::Mesh mesh;
      jsonget(data["meshes"][i], mesh, name);
      if (data["meshes"][i].contains("primitives"))
      {
        mesh.primitives = std::vector<gltf::Mesh::Primitive>();
        for (int j = 0; j < data["meshes"][i]["primitives"].size(); j++)
        {
          gltf::Mesh::Primitive prim;
          jsonget(data["meshes"][i]["primitives"][j], prim, mode);
          jsonget(data["meshes"][i]["primitives"][j], prim, indices);
          if (data["meshes"][i]["primitives"][j].contains("attributes"))
          {
            jsonget(data["meshes"][i]["primitives"][j]["attributes"], prim.attributes, POSITION);
            jsonget(data["meshes"][i]["primitives"][j]["attributes"], prim.attributes, NORMAL);
            jsonget(data["meshes"][i]["primitives"][j]["attributes"], prim.attributes, TANGENT);
            jsonget(data["meshes"][i]["primitives"][j]["attributes"], prim.attributes, TEXCOORD_0);
            jsonget(data["meshes"][i]["primitives"][j]["attributes"], prim.attributes, TEXCOORD_1);
            jsonget(data["meshes"][i]["primitives"][j]["attributes"], prim.attributes, TEXCOORD_2);
            jsonget(data["meshes"][i]["primitives"][j]["attributes"], prim.attributes, COLOR_0);
            jsonget(data["meshes"][i]["primitives"][j]["attributes"], prim.attributes, JOINTS_0);
            jsonget(data["meshes"][i]["primitives"][j]["attributes"], prim.attributes, WEIGHTS_0);
          }
          jsonget(data["meshes"][i]["primitives"][j], prim, material);
          prim.targets = std::vector<gltf::Mesh::Primitive::MorphTarget>();
          for (int k = 0; k < data["meshes"][i]["primitives"][j]["targets"].size(); k++)
          {
            gltf::Mesh::Primitive::MorphTarget mt;
            jsonget(data["meshes"][i]["primitives"][j]["targets"][k], mt, POSITION);
            jsonget(data["meshes"][i]["primitives"][j]["targets"][k], mt, NORMAL);
            jsonget(data["meshes"][i]["primitives"][j]["targets"][k], mt, TANGENT);
            prim.targets.push_back(mt);
          }
          prim.extensions = deserialize_extensions(data["meshes"][i]["primitives"][j]);
          prim.extras = deserialize_extras(data["meshes"][i]["primitives"][j]);
          mesh.primitives.push_back(prim);
        }
      }
      jsonget(data["meshes"][i], mesh, weights);
      mesh.extensions = deserialize_extensions(data["meshes"][i]);
      mesh.extras = deserialize_extras(data["meshes"][i]);
      gltf.meshes.push_back(mesh);
    }
  }
  if (data.contains("nodes"))
  {
    for (int i = 0; i < data["nodes"].size(); i++)
    {
      gltf::Node node;
      auto nodeData = data["nodes"][i];
      jsonget(nodeData, node, name);
      jsonget(nodeData, node, children);
      if (nodeData.contains("matrix"))
      {
        std::vector<float> tmp = std::vector<float>();
        tmp = nodeData["matrix"].get<typeof(tmp)>();
        memcpy(node.matrix, tmp.data(), sizeof(float)*tmp.size());
      }
      if (nodeData.contains("translation"))
      {
        std::vector<float> tmp = std::vector<float>();
        tmp = nodeData["translation"].get<typeof(tmp)>();
        memcpy(node.translation, tmp.data(), sizeof(float) * tmp.size());
      }
      if (nodeData.contains("rotation"))
      {
        std::vector<float> tmp = std::vector<float>();
        tmp = nodeData["rotation"].get<typeof(tmp)>();
        memcpy(node.rotation, tmp.data(), sizeof(float) * tmp.size());
      }
      if (nodeData.contains("scale"))
      {
        std::vector<float> tmp = std::vector<float>();
        tmp = nodeData["scale"].get<typeof(tmp)>();
        memcpy(node.scale, tmp.data(), sizeof(float) * tmp.size());
      }
      jsonget(nodeData, node, mesh);
      jsonget(nodeData, node, skin);
      jsonget(nodeData, node, weights);
      jsonget(nodeData, node, camera);
      jsongetEx(nodeData, node);
      gltf.nodes.push_back(node);
    }
  }
  if (data.contains("skins"))
  {
    for (int i = 0; i < data["skins"].size(); i++)
    {
      gltf::Skin skin;
      jsonget(data["skins"][i], skin, name);
      jsonget(data["skins"][i], skin, inverseBindMatrices);
      jsonget(data["skins"][i], skin, joints);
      jsonget(data["skins"][i], skin, skeleton);
      jsongetEx(data["skins"][i], skin);
      gltf.skins.push_back(skin);
    }
  }
  jsonget(data, gltf, scene);
  if (data.contains("scenes"))
  {
    for (int i = 0; i < data["scenes"].size(); i++)
    {
      gltf::Scene sc;
      jsonget(data["scenes"][i], sc, name);
      jsonget(data["scenes"][i], sc, nodes);
      jsongetEx(data["scenes"][i], sc);
      gltf.scenes.push_back(sc);
    }
  }
  if (data.contains("animations"))
  {
    for (int i = 0; i < data["animations"].size(); i++)
    {
      gltf::Animation an;
      auto animData = data["animations"][i];
      jsonget(animData, an, name);
      an.channels = std::vector<gltf::Animation::AnimationChannel>();
      for (int j = 0; j < animData["channels"].size(); j++)
      {
        gltf::Animation::AnimationChannel ch;
        jsonget(animData["channels"][j], ch, sampler);
        if (animData["channels"][j].contains("target"))
        {
          jsonget(animData["channels"][j]["target"], ch.target, node);
          jsonget(animData["channels"][j]["target"], ch.target, path);
          jsongetEx(animData["channels"][j]["target"], ch.target);
        }
        jsongetEx(animData["channels"][j], ch);
        an.channels.push_back(ch);
      }
      for (int j = 0; j < animData["samplers"].size(); j++)
      {
        gltf::Animation::AnimationSampler sam;
        jsonget(animData["sampler"][j], sam, input);
        jsonget(animData["sampler"][j], sam, interpolation);
        jsonget(animData["sampler"][j], sam, output);
        jsongetEx(animData["sampler"][j], sam);
        an.samplers.push_back(sam);
      }
      jsongetEx(animData, an);
      gltf.animations.push_back(an);
    }
  }
  if (data.contains("cameras"))
  {
    for (int i = 0; i < data["cameras"].size(); i++)
    {
      gltf::Camera cam;
      jsonget(data["cameras"][i], cam, name);
      if (data["cameras"][i].contains("orthographic"))
      {
        jsonget(data["cameras"][i]["orthographic"], cam.orthographic, xmag);
        jsonget(data["cameras"][i]["orthographic"], cam.orthographic, ymag);
        jsonget(data["cameras"][i]["orthographic"], cam.orthographic, zfar);
        jsonget(data["cameras"][i]["orthographic"], cam.orthographic, znear);
        jsongetEx(data["cameras"][i]["orthographic"], cam.orthographic);
      }
      if (data["cameras"][i].contains("perspective"))
      {
        jsonget(data["cameras"][i]["perspective"], cam.perspective, aspectRatio);
        jsonget(data["cameras"][i]["perspective"], cam.perspective, yfov);
        jsonget(data["cameras"][i]["perspective"], cam.perspective, zfar);
        jsonget(data["cameras"][i]["perspective"], cam.perspective, znear);
      }
      jsongetEnum(data["cameras"][i], cam, type, gltf::Camera::ProjectionType::, Perspective, Orthographic);
      jsongetEx(data["cameras"][i], cam);
      gltf.cameras.push_back(cam);
    }
  }
  jsonget(data, gltf, extensionsUsed);
  jsonget(data, gltf, extensionsRequired);
  jsongetEx(data, gltf);
  return gltf;
}
#undef jsongetEx
#undef jsongetEnum
#undef jsonget

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
    // TODO(ANT) images ig
  }

  return model;
}
static gltf::glTFModel parseGLB(std::string path, BIN_t bin)
{
  gltf::glTFModel model;
  if (bin.size <= 12)
  {
    chprinterr("Error loading file at %s:%d.\nFile seems to be empty\n", __FILE__, __LINE__);
  }
  membuild(
      struct {
        char glTF[4];
        uint32_t version;
        uint32_t dataSize;
      },
      header, pop_buff(bin.data, 12));
  printf("Loading model:%s\nglTF version %d size: %d, magic: %s\n", path.c_str(), header.version, header.dataSize, std::string(header.glTF, 4).c_str());
  if (memcmp("glTF", header.glTF, 4))
  {
    chprinterr("Error loading file at %s:%d.\nUnexpected magic: %s. File might be corrupted\n", __FILE__, __LINE__, header.glTF);
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
  chunk_t chunks[2] = {{0}, {0}};
  while (dataSize < header.dataSize)
  {
    chunk_t c = {CHVAL, CHVAL, 0};
    memcpy(&c, pop_buff(bin.data, 8), 8);
    chassert(c.length != CHVAL && c.type != CHVAL, "Could not read data length or type from buffer")
        c.data = (uchar *)malloc(c.length);
    memcpy(c.data, pop_buff(bin.data, c.length), c.length);
    dataSize += 8 + c.length;
    if (c.type == JSON_GLB_CHUNK)
      chunks[0] = c;
    else if (c.type == BIN_GLB_CHUNK)
      chunks[1] = c;
    else
      chprinterr("Error during loading file chunk %d at %s:%d.\nUnexpected chunk type:%X. File might be corrupted\n", nChunks, __FILE__, __LINE__, c.type);
  }

  json jsonData = json::parse(chunks[0].data, chunks[0].data + chunks[0].length);
  model = parseGLTFJSON(jsonData);

  if (model.buffers.size() != 1)
  {
    chprinterr("Only 1 buffer should be present in file");
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
    chprinterr("Could not open file:%s\n", path.c_str());
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
