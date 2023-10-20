#include "loader.hpp"
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

static std::vector<gltf::Extension> deserialize_extensions(json extensions);
static std::vector<gltf::Extension> deserialize_extras(json extensions);
static std::vector<gltf::Extension> deserialize_extensions(json extensions)
{
#define jsonContains(item, type, name, store) \
  if (item.contains(#name))                   \
  {                                           \
    (store).name = item[#name].get<type>();   \
  }
  std::vector<gltf::Extension> vec;
  for (auto &x : extensions.items())
  {
    gltf::Extension ext;
    ext.name = x.key();
    if (ext.name == gltf::SUPPORTED_EXTENSIONS.KHR_materials_unlit)
    {
      ext.data = new gltf::Extensions::KHR_materials_unlit();
    }
    else if (ext.name == gltf::SUPPORTED_EXTENSIONS.KHR_texture_transform)
    {
      gltf::Extensions::KHR_texture_transform *transform = new gltf::Extensions::KHR_texture_transform();
      for (auto &y : x.value().items())
      {
        if (std::string(y.key()) == "offset")
        {
          for (uint i = 0; i < y.value().size(); i++)
          {
            transform->offset[i] = y.value()[i].get<float>();
          }
          continue;
        }
        if (std::string(y.key()) == "rotation")
        {
          transform->rotation = y.value().get<float>();
          continue;
        }
        if (std::string(y.key()) == "scale")
        {
          for (uint i = 0; i < y.value().size(); i++)
          {
            transform->scale[i] = y.value()[i].get<float>();
          }
          continue;
        }
        if (std::string(y.key()) == "texCoord")
        {
          transform->texCoord = y.value().get<int>();
          continue;
        }
      }
      ext.data = transform;
    }
    else if (ext.name == gltf::SUPPORTED_EXTENSIONS.KHR_materials_emissive_strength)
    {
      gltf::Extensions::KHR_materials_emissive_strength *ems = new gltf::Extensions::KHR_materials_emissive_strength();
      jsonContains(x.value(), float, emissiveStrength, *ems);
      ext.data = ems;
    }
    else if (ext.name == gltf::SUPPORTED_EXTENSIONS.VRM)
    {
      fprintf(stderr, "vrm 0.0 not supported\n");
      // exit(1);
      // gltf::Extensions::VRM tmpExt = deserializeVRM(x.value());
      gltf::Extensions::VRM *vrm = new gltf::Extensions::VRM();
      // *vrm = tmpExt;
      ext.data = vrm;
    }
    else if (ext.name == gltf::SUPPORTED_EXTENSIONS.VRMC_springBone)
    {
      gltf::Extensions::VRMC_springBone *springBone = new gltf::Extensions::VRMC_springBone();
      springBone->specVersion = x.value()["specVersion"].get<std::string>();
      springBone->colliders = std::vector<gltf::Extensions::VRMC_springBone::Collider>();
      if (x.value().contains("colliders"))
      {
        for (int i = 0; i < x.value()["colliders"].size(); i++)
        {
          auto &y = x.value()["colliders"][i];
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
      if (x.value().contains("colliderGroups"))
      {
        for (int i = 0; i < x.value()["colliderGroups"].size(); i++)
        {
          auto &y = x.value()["colliderGroups"][i];
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
      if (x.value().contains("springs"))
      {
        gltf::Extensions::VRMC_springBone::Spring spr;
        for (int i = 0; i < x.value()["springs"].size(); i++)
        {
          auto &y = x.value()["springs"][i];
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
        }
        springBone->springs.push_back(spr);
      }
    }
    else if (ext.name == gltf::SUPPORTED_EXTENSIONS.VRMC_materials_mtoon)
    {
      gltf::Extensions::VRMC_materials_mtoon *mtoon = new gltf::Extensions::VRMC_materials_mtoon();
      auto &item = x.value();
      jsonContains(item, std::string, specVersion, *mtoon);
      jsonContains(item, bool, transparentWithZWrite, *mtoon);
      jsonContains(item, int, renderQueueOffsetNumber, *mtoon);
      if (item.contains("shadeColorFactor"))
      {
        mtoon->shadeColorFactor[0] = item["shadeColorFactor"][0].get<float>();
        mtoon->shadeColorFactor[1] = item["shadeColorFactor"][1].get<float>();
        mtoon->shadeColorFactor[2] = item["shadeColorFactor"][2].get<float>();
      }
      if (item.contains("shadeMultiplyTexture"))
      {
        jsonContains(item["shadeMultiplyTexture"], int, index, mtoon->shadeMultiplyTexture);
        jsonContains(item["shadeMultiplyTexture"], int, texCoord, mtoon->shadeMultiplyTexture);
        if (item["shadeMultiplyTexture"].contains("extensions"))
        {
          mtoon->shadeMultiplyTexture.extensions = deserialize_extensions(item["shadeMultiplyTexture"]["extensions"]);
        }
        if (item["shadeMultiplyTexture"].contains("extras"))
        {
          mtoon->shadeMultiplyTexture.extras = deserialize_extras(item["shadeMultiplyTexture"]["extras"]);
        }
      }
      jsonContains(item, float, shadingShiftFactor, *mtoon);
      if (item.contains("shadingShiftTexture"))
      {
        jsonContains(item["shadingShiftTexture"], int, index, mtoon->shadingShiftTexture);
        jsonContains(item["shadingShiftTexture"], int, texCoord, mtoon->shadingShiftTexture);
        jsonContains(item["shadingShiftTexture"], float, scale, mtoon->shadingShiftTexture);
        if (item["shadingShiftTexture"].contains("extensions"))
        {
          mtoon->shadingShiftTexture.extensions = deserialize_extensions(item["shadingShiftTexture"]["extensions"]);
        }
        if (item["shadingShiftTexture"].contains("extras"))
        {
          mtoon->shadingShiftTexture.extras = deserialize_extras(item["shadingShiftTexture"]["extras"]);
        }
      }
      jsonContains(item, float, shadingToonyFactor, *mtoon);
      jsonContains(item, float, giEqualizationFactor, *mtoon);
      if (item.contains("matcapFactor"))
      {
        mtoon->matcapFactor[0] = item["matcapFactor"][0].get<float>();
        mtoon->matcapFactor[1] = item["matcapFactor"][1].get<float>();
        mtoon->matcapFactor[2] = item["matcapFactor"][2].get<float>();
      }
      if (item.contains("matcapTexture"))
      {
        jsonContains(item["matcapTexture"], int, index, mtoon->matcapTexture);
        jsonContains(item["matcapTexture"], int, texCoord, mtoon->matcapTexture);
        if (item["matcapTexture"].contains("extensions"))
        {
          mtoon->matcapTexture.extensions = deserialize_extensions(item["matcapTexture"]["extensions"]);
        }
        if (item["matcapTexture"].contains("extras"))
        {
          mtoon->matcapTexture.extras = deserialize_extras(item["matcapTexture"]["extras"]);
        }
      }
      if (item.contains("parametricRimColorFactor"))
      {
        mtoon->parametricRimColorFactor[0] = item["parametricRimColorFactor"][0].get<float>();
        mtoon->parametricRimColorFactor[1] = item["parametricRimColorFactor"][1].get<float>();
        mtoon->parametricRimColorFactor[2] = item["parametricRimColorFactor"][2].get<float>();
      }
      if (item.contains("rimMultiplyTexture"))
      {
        jsonContains(item["rimMultiplyTexture"], int, index, mtoon->rimMultiplyTexture);
        jsonContains(item["rimMultiplyTexture"], int, texCoord, mtoon->rimMultiplyTexture);
        if (item["rimMultiplyTexture"].contains("extensions"))
        {
          mtoon->rimMultiplyTexture.extensions = deserialize_extensions(item["rimMultiplyTexture"]["extensions"]);
        }
        if (item["rimMultiplyTexture"].contains("extras"))
        {
          mtoon->rimMultiplyTexture.extras = deserialize_extras(item["rimMultiplyTexture"]["extras"]);
        }
      }
      jsonContains(item, float, rimLightingMixFactor, *mtoon);
      jsonContains(item, float, parametricRimFresnelPowerFactor, *mtoon);
      jsonContains(item, float, parametricRimLiftFactor, *mtoon);
      if (item.contains("outlineWidthMode"))
      {
        mtoon->outlineWidthMode = (item["outlineWidthMode"].get<std::string>() == "worldCoordinates" ? gltf::Extensions::VRMC_materials_mtoon::OutlineWidthMode::worldCoordinates : (item["outlineWidthMode"].get<std::string>() == "screenCoordinates" ? gltf::Extensions::VRMC_materials_mtoon::OutlineWidthMode::screenCoordinates : gltf::Extensions::VRMC_materials_mtoon::OutlineWidthMode::none));
      }
      jsonContains(item, float, outlineWidthFactor, *mtoon);
      if (item.contains("outlineWidthMultiplyTexture"))
      {
        jsonContains(item["outlineWidthMultiplyTexture"], int, index, mtoon->outlineWidthMultiplyTexture);
        jsonContains(item["outlineWidthMultiplyTexture"], int, texCoord, mtoon->outlineWidthMultiplyTexture);
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
      jsonContains(item, float, outlineLightingMixFactor, *mtoon);
      if (item.contains("uvAnimationMaskTexture"))
      {
        jsonContains(item["uvAnimationMaskTexture"], int, index, mtoon->uvAnimationMaskTexture);
        jsonContains(item["uvAnimationMaskTexture"], int, texCoord, mtoon->uvAnimationMaskTexture);
        if (item["uvAnimationMaskTexture"].contains("extensions"))
        {
          mtoon->uvAnimationMaskTexture.extensions = deserialize_extensions(item["uvAnimationMaskTexture"]["extensions"]);
        }
        if (item["uvAnimationMaskTexture"].contains("extras"))
        {
          mtoon->uvAnimationMaskTexture.extras = deserialize_extras(item["uvAnimationMaskTexture"]["extras"]);
        }
      }
      jsonContains(item, float, uvAnimationScrollXSpeedFactor, *mtoon);
      jsonContains(item, float, uvAnimationScrollYSpeedFactor, *mtoon);
      jsonContains(item, float, uvAnimationRotationSpeedFactor, *mtoon);
      ext.data = mtoon;
    }
    else if (ext.name == gltf::SUPPORTED_EXTENSIONS.VRMC_node_constraint)
    {
      gltf::Extensions::VRMC_node_constraint *node = new gltf::Extensions::VRMC_node_constraint();
      auto &item = x.value();
      jsonContains(item, std::string, specVersion, *node);
      if (item["constraint"].contains("roll"))
      {
        node->constraint.roll.source = item["constraint"]["roll"]["source"].get<int>();
        std::string rollAxis = item["constraint"]["roll"]["rollAxis"].get<std::string>();
        node->constraint.roll.rollAxis = (rollAxis == "X" ? node->constraint.roll.X : (rollAxis == "Y" ? node->constraint.roll.Y : node->constraint.roll.Z));
        jsonContains(item["constraint"]["roll"], float, weight, node->constraint.roll);
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
        jsonContains(item["constraint"]["aim"], float, weight, node->constraint.aim);
      }
      else if (item["constraint"].contains("rotation"))
      {
        node->constraint.rotation.source = item["constraint"]["rotation"]["source"].get<int>();
        jsonContains(item["constraint"]["rotation"], float, weight, node->constraint.rotation);
      }
      ext.data = node;
    }
    else if (ext.name == gltf::SUPPORTED_EXTENSIONS.VRMC_vrm)
    {
      gltf::Extensions::VRMC_vrm *vrm = new gltf::Extensions::VRMC_vrm();
      auto &item = x.value();
      vrm->specVersion = item["specVersion"].get<std::string>();
      vrm->meta.name = item["meta"]["name"];
      jsonContains(item["meta"], std::string, version, vrm->meta);
      vrm->meta.authors = std::vector<std::string>();
      for (int i = 0; i < item["meta"]["authors"].size(); i++)
      {
        vrm->meta.authors.push_back(item["meta"]["authors"][i].get<std::string>());
      }
      jsonContains(item["meta"], std::string, copyrightInformation, vrm->meta);
      jsonContains(item["meta"], std::string, contactInformation, vrm->meta);
      vrm->meta.reference = std::vector<std::string>();
      if (item["meta"].contains("reference"))
      {
        for (int i = 0; i < item["meta"]["reference"].size(); i++)
        {
          vrm->meta.reference.push_back(item["meta"]["reference"][i].get<std::string>());
        }
      }
      jsonContains(item["meta"], std::string, thirdPartyLicenses, vrm->meta);
      vrm->meta.licenseUrl = item["meta"]["licenseUrl"].get<std::string>();
      if (item["meta"].contains("avatarPermission"))
      {
        std::string str = item["meta"]["avatarPermission"];
        if (str == "onlyAuthor")
        {
          vrm->meta.avatarPermission = vrm->meta.onlyAuthor;
        }
        else if (str == "onlySeparatelyLicensedPerson")
        {
          vrm->meta.avatarPermission = vrm->meta.onlySeparatelyLicensedPerson;
        }
        else if (str == "everyone")
        {
          vrm->meta.avatarPermission = vrm->meta.everyone;
        }
      }
      jsonContains(item["meta"], bool, allowExcessivelyViolentUsage, vrm->meta);
      jsonContains(item["meta"], bool, allowExcessivelySexualUsage, vrm->meta);
      if (item["meta"].contains("commercialUsage"))
      {
        std::string str = item["meta"]["commercialUsage"].get<std::string>();
        if (str == "personalNonProfit")
        {
          vrm->meta.commercialUsage = vrm->meta.personalNonProfit;
        }
        else if (str == "personalProfit")
        {
          vrm->meta.commercialUsage = vrm->meta.personalProfit;
        }
        else if (str == "corporation")
        {
          vrm->meta.commercialUsage = vrm->meta.corporation;
        }
      }
      jsonContains(item["meta"], bool, allowPoliticalOrReligiousUsage, vrm->meta);
      jsonContains(item["meta"], bool, allowAntisocialOrHateUsage, vrm->meta);
      if (item["meta"].contains("creditNotation"))
      {
        std::string str = item["meta"]["creditNotation"].get<std::string>();
        if (str == "required")
        {
          vrm->meta.creditNotation = vrm->meta.required;
        }
        else if (str == "unnecessary")
        {
          vrm->meta.creditNotation = vrm->meta.unnecessary;
        }
      }
      jsonContains(item["meta"], bool, allowRedistribution, vrm->meta);
      if (item["meta"].contains("modification"))
      {
        std::string str = item["meta"]["modification"].get<std::string>();
        if (str == "prohibited")
        {
          vrm->meta.modification = vrm->meta.prohibited;
        }
        else if (str == "allowModification")
        {
          vrm->meta.modification = vrm->meta.allowModification;
        }
        else if (str == "allowModificationRedistribution")
        {
          vrm->meta.modification = vrm->meta.allowModificationRedistribution;
        }
        jsonContains(item["meta"], std::string, otherLicenseUrl, vrm->meta);
      }
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
          jsonContains(item["lookAt"]["rangeMapHorizontalInner"], float, inputMaxValue, vrm->lookAt.rangeMapHorizontalInner);
          jsonContains(item["lookAt"]["rangeMapHorizontalInner"], float, outputScale, vrm->lookAt.rangeMapHorizontalInner);
        }
        if (item["lookAt"].contains("rangeMapHorizontalOuter"))
        {
          jsonContains(item["lookAt"]["rangeMapHorizontalOuter"], float, inputMaxValue, vrm->lookAt.rangeMapHorizontalOuter);
          jsonContains(item["lookAt"]["rangeMapHorizontalOuter"], float, outputScale, vrm->lookAt.rangeMapHorizontalOuter);
        }
        if (item["lookAt"].contains("rangeMapVerticalDown"))
        {
          jsonContains(item["lookAt"]["rangeMapVerticalDown"], float, inputMaxValue, vrm->lookAt.rangeMapVerticalDown);
          jsonContains(item["lookAt"]["rangeMapVerticalDown"], float, outputScale, vrm->lookAt.rangeMapVerticalDown);
        }
        if (item["lookAt"].contains("rangeMapVerticalUp"))
        {
          jsonContains(item["lookAt"]["rangeMapVerticalUp"], float, inputMaxValue, vrm->lookAt.rangeMapVerticalUp);
          jsonContains(item["lookAt"]["rangeMapVerticalUp"], float, outputScale, vrm->lookAt.rangeMapVerticalUp);
        }
      }
      if (item.contains("expressions"))
      {
        if (item["expressions"].contains("preset"))
        {
#define BlockBlendParse(dst, src)                                           \
  std::string str = src;                                                    \
  if (str == "none")                                                        \
  {                                                                         \
    dst = gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::none;  \
  }                                                                         \
  else if (str == "block")                                                  \
  {                                                                         \
    dst = gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::block; \
  }                                                                         \
  else if (str == "blend")                                                  \
  {                                                                         \
    dst = gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::blend; \
  }
#define ExpressionParse(EXP)                                                                                                                                                \
  if (item["expressions"]["preset"].contains(#EXP))                                                                                                                          \
  {                                                                                                                                                                         \
    if (item["expressions"]["preset"][#EXP].contains("morphTargetBinds"))                                                                                                    \
    {                                                                                                                                                                       \
      vrm->expressions.preset.EXP.morphTargetBinds =                                                                                                                        \
          std::vector<gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::MorphTargetBind>(item["expressions"]["preset"][#EXP]["morphTargetBinds"].size());           \
      for (int i = 0; i < item["expressions"]["preset"][#EXP]["morphTargetBinds"].size(); i++)                                                                               \
      {                                                                                                                                                                     \
        vrm->expressions.preset.EXP.morphTargetBinds[i].node = item["expressions"]["preset"][#EXP]["morphTargetBinds"][i]["node"].get<int>();                                \
        vrm->expressions.preset.EXP.morphTargetBinds[i].index = item["expressions"]["preset"][#EXP]["morphTargetBinds"][i]["index"].get<int>();                              \
        vrm->expressions.preset.EXP.morphTargetBinds[i].weight = item["expressions"]["preset"][#EXP]["morphTargetBinds"][i]["weight"].get<float>();                          \
      }                                                                                                                                                                     \
    }                                                                                                                                                                       \
    if (item["expressions"]["preset"][#EXP].contains("materialColorBinds"))                                                                                                  \
    {                                                                                                                                                                       \
      vrm->expressions.preset.EXP.materialColorBinds =                                                                                                                      \
          std::vector<gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::MaterialColorBind>(item["expressions"]["preset"][#EXP]["materialColorBinds"].size());       \
                                                                                                                                                                            \
      for (int i = 0; i < item["expressions"]["preset"][#EXP]["materialColorBinds"].size(); i++)                                                                             \
      {                                                                                                                                                                     \
        vrm->expressions.preset.EXP.materialColorBinds[i].material = item["expressions"]["preset"][#EXP]["materialColorBinds"][i]["material"].get<int>();                    \
        std::string str = item["expressions"]["preset"][#EXP]["materialColorBinds"][i]["type"].get<std::string>();                                                           \
        if (str == "color")                                                                                                                                                 \
        {                                                                                                                                                                   \
          vrm->expressions.preset.EXP.materialColorBinds[i].type = gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::MaterialColorBind::color;                     \
        }                                                                                                                                                                   \
        else if (str == "emissionColor")                                                                                                                                    \
        {                                                                                                                                                                   \
          vrm->expressions.preset.EXP.materialColorBinds[i].type = gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::MaterialColorBind::emissionColor;             \
        }                                                                                                                                                                   \
        else if (str == "shadeColor")                                                                                                                                       \
        {                                                                                                                                                                   \
          vrm->expressions.preset.EXP.materialColorBinds[i].type = gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::MaterialColorBind::shadeColor;                \
        }                                                                                                                                                                   \
        else if (str == "matcapColor")                                                                                                                                      \
        {                                                                                                                                                                   \
          vrm->expressions.preset.EXP.materialColorBinds[i].type = gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::MaterialColorBind::matcapColor;               \
        }                                                                                                                                                                   \
        else if (str == "rimColor")                                                                                                                                         \
        {                                                                                                                                                                   \
          vrm->expressions.preset.EXP.materialColorBinds[i].type = gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::MaterialColorBind::rimColor;                  \
        }                                                                                                                                                                   \
        else if (str == "outlineColor")                                                                                                                                     \
        {                                                                                                                                                                   \
          vrm->expressions.preset.EXP.materialColorBinds[i].type = gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::MaterialColorBind::outlineColor;              \
        }                                                                                                                                                                   \
        vrm->expressions.preset.EXP.materialColorBinds[i].targetValue[0] = item["expressions"]["preset"][#EXP]["materialColorBinds"][i]["targetValue"][0].get<float>();      \
        vrm->expressions.preset.EXP.materialColorBinds[i].targetValue[1] = item["expressions"]["preset"][#EXP]["materialColorBinds"][i]["targetValue"][1].get<float>();      \
        vrm->expressions.preset.EXP.materialColorBinds[i].targetValue[2] = item["expressions"]["preset"][#EXP]["materialColorBinds"][i]["targetValue"][2].get<float>();      \
        vrm->expressions.preset.EXP.materialColorBinds[i].targetValue[3] = item["expressions"]["preset"][#EXP]["materialColorBinds"][i]["targetValue"][3].get<float>();      \
      }                                                                                                                                                                     \
    }                                                                                                                                                                       \
    if (item["expressions"]["preset"][#EXP].contains("textureTransformBinds"))                                                                                               \
    {                                                                                                                                                                       \
      vrm->expressions.preset.EXP.textureTransformBinds =                                                                                                                   \
          std::vector<gltf::Extensions::VRMC_vrm::ExpressionPresets::Expression::TextureTransformBind>(item["expressions"]["preset"][#EXP]["textureTransformBinds"].size()); \
      for (int i = 0; i < item["expressions"]["preset"][#EXP]["textureTransformBinds"].size(); i++)                                                                          \
      {                                                                                                                                                                     \
        vrm->expressions.preset.EXP.textureTransformBinds[i].material = item["expressions"]["preset"][#EXP]["textureTransformBinds"][i]["material"].get<int>();              \
        if (item["expressions"]["preset"][#EXP]["textureTransformBinds"][i].contains("scale"))                                                                               \
        {                                                                                                                                                                   \
          vrm->expressions.preset.EXP.textureTransformBinds[i].scale[0] = item["expressions"]["preset"][#EXP]["textureTransformBinds"][i]["scale"][0].get<float>();          \
          vrm->expressions.preset.EXP.textureTransformBinds[i].scale[1] = item["expressions"]["preset"][#EXP]["textureTransformBinds"][i]["scale"][1].get<float>();          \
        }                                                                                                                                                                   \
        if (item["expressions"]["preset"][#EXP]["textureTransformBinds"][i].contains("offset"))                                                                              \
        {                                                                                                                                                                   \
          vrm->expressions.preset.EXP.textureTransformBinds[i].offset[0] = item["expressions"]["preset"][#EXP]["textureTransformBinds"][i]["offset"][0].get<float>();        \
          vrm->expressions.preset.EXP.textureTransformBinds[i].offset[1] = item["expressions"]["preset"][#EXP]["textureTransformBinds"][i]["offset"][1].get<float>();        \
        }                                                                                                                                                                   \
      }                                                                                                                                                                     \
    }                                                                                                                                                                       \
    if (item["expressions"]["preset"][#EXP].contains("isBinary"))                                                                                                            \
    {                                                                                                                                                                       \
      vrm->expressions.preset.EXP.isBinary = item["expressions"]["preset"][#EXP]["isBinary"].get<bool>();                                                                    \
    }                                                                                                                                                                       \
    if (item["expressions"]["preset"][#EXP].contains("overrideBlink"))                                                                                                       \
    {                                                                                                                                                                       \
      BlockBlendParse(vrm->expressions.preset.EXP.overrideBlink, item["expressions"]["preset"][#EXP]["overrideBlink"].get<std::string>());                                   \
    }                                                                                                                                                                       \
    if (item["expressions"]["preset"][#EXP].contains("overrideLookAt"))                                                                                                      \
    {                                                                                                                                                                       \
      BlockBlendParse(vrm->expressions.preset.EXP.overrideLookAt, item["expressions"]["preset"][#EXP]["overrideLookAt"].get<std::string>());                                 \
    }                                                                                                                                                                       \
    if (item["expressions"]["preset"][#EXP].contains("overrideMouth"))                                                                                                       \
    {                                                                                                                                                                       \
      BlockBlendParse(vrm->expressions.preset.EXP.overrideLookAt, item["expressions"]["preset"][#EXP]["overrideMouth"].get<std::string>());                                  \
    }                                                                                                                                                                       \
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
#undef BlockBlendParse
        }
      }
      ext.data = vrm;
    }
    else
    {
      fprintf(stderr, "%s: extension not supported\n", x.key().c_str());
      exit(1);
      continue;
    }
    vec.push_back(ext);
  }
#undef jsonContains
  return vec;
}
static std::vector<gltf::Extension> deserialize_extras(json extras)
{
  std::vector<gltf::Extension> vec;
  if (extras.contains("targetNames"))
  {
    gltf::Extension ext;
    ext.name = "targetNames";
    gltf::Extras::TargetNames *targetNames = new gltf::Extras::TargetNames();
    targetNames->targetNames = std::vector<std::string>();
    for (int i = 0; i < extras["targetNames"].size(); i++)
    {
      targetNames->targetNames.push_back(extras["targetNames"][i].get<std::string>());
    }
    ext.data = (void *)targetNames;
    vec.push_back(ext);
  }
  return vec;
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
#define jsonUnknownMacro(item, location)                                                                      \
  fprintf(stderr, "%s:%d: %s unknow item in object %s\n", __FILE__, __LINE__, item.key().c_str(), #location); \
  exit(1);

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
            for (auto &s : x.value().items())
            {
              jsonConvertMacro(s, accessor.sparse, count, int);
              if (std::string(s.key()) == "indices")
              {
                for (auto &y : s.value().items())
                {
                  jsonConvertMacro(y, accessor.sparse.indices, bufferView, int);
                  jsonConvertMacro(y, accessor.sparse.indices, byteOffset, int);
                  jsonConvertCastMacro(y, accessor.sparse.indices, componentType, int, gltf::Accessor::Sparse::Indices::glComponentType)
                      jsonExtensionMacro(y, accessor.sparse.indices, extensions);
                  jsonExtensionMacro(y, accessor.sparse.indices, extras);
                }
                continue;
              }
              if (std::string(s.key()) == "values")
              {
                for (auto &y : s.value().items())
                {
                  jsonConvertMacro(y, accessor.sparse.values, bufferView, int);
                  jsonConvertMacro(y, accessor.sparse.values, byteOffset, int);
                  jsonExtensionMacro(y, accessor.sparse.values, extensions);
                  jsonExtensionMacro(y, accessor.sparse.values, extras);
                }
                continue;
              }
              jsonExtensionMacro(s, accessor.sparse, extensions);
              jsonExtensionMacro(s, accessor.sparse, extras);
              jsonUnknownMacro(s, model.accessors.sparse) continue;
            }
            continue;
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
                  if (mesh.weights.size() != primitive.targets.size())
                    mesh.weights = std::vector<float>(primitive.targets.size());
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
          if (std::string(x.key()) == "matrix")
          {
            for (int j = 0; j < x.value().size(); j++)
            {
              node.matrix[j] = x.value()[j].get<float>();
            }
            continue;
          }
          if (std::string(x.key()) == "translation")
          {
            for (int j = 0; j < x.value().size(); j++)
            {
              node.translation[j] = x.value()[j].get<float>();
            }
            continue;
          }
          if (std::string(x.key()) == "rotation")
          {
            for (int j = 0; j < x.value().size(); j++)
            {
              node.rotation[j] = x.value()[j].get<float>();
            }
            continue;
          }
          if (std::string(x.key()) == "scale")
          {
            for (int j = 0; j < x.value().size(); j++)
            {
              node.scale[j] = x.value()[j].get<float>();
            };
            continue;
          };
          jsonConvertMacro(x, node, mesh, int);
          jsonConvertMacro(x, node, skin, int);
          jsonArrayMacro(x, node, weights, float, j);
          jsonConvertMacro(x, node, camera, int);
          jsonExtensionMacro(x, node, extensions);
          jsonExtensionMacro(x, node, extras);
          jsonUnknownMacro(x, model.nodes);
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
  printf("Loading model:%s\nglTF version %d size: %d, magic: %s\n", path.c_str(), header.version, header.dataSize, std::string(header.glTF,4).c_str());
  if (memcmp("glTF", header.glTF, 4))
  {
    fprintf(stderr, "Error loading file at %s:%d.\nUnexpected magic: %s. File might be corrupted\n", __FILE__, __LINE__, header.glTF);
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
      fprintf(stderr, "Error during loading file chunk %d at %s:%d.\nUnexpected chunk type:%X. File might be corrupted\n", nChunks, __FILE__, __LINE__, c.type);
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
