#include "vmodelPhysics.hpp"
#include "../loader/vanche_model.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#define v3(V) (glm::vec3(V[0], V[1], V[2]))
#define v4(V) (glm::vec4(V[0], V[1], V[2], V[3]))
#define v4quat(V) (glm::quat(V[3], V[0], V[1], V[2]))

static int vrmConstraints(VModel_t *vmodel)
{
  for (int i = 0; i < vmodel->model.nodes.size(); i++)
  {
    gltf::Node &node = vmodel->model.nodes[i];
    gltf::Extensions::VRMC_node_constraint *constraint = ch_hashget(gltf::Extensions::VRMC_node_constraint *, node.extensions, gltf::SUPPORTED_EXTENSIONS.VRMC_node_constraint);
    if (constraint)
    {
      if (constraint->constraint.aim.source != -1)
      {
        uint _src = constraint->constraint.aim.source;
        glm::mat4 worldMat = vmodel->physics.nodeMats[i] * glm::inverse(getNodeTransform(vmodel, i, glm::mat4(1)));

        glm::vec4 fromVec = glm::vec4(0);
        switch (constraint->constraint.aim.aimAxis)
        {
        case constraint->constraint.aim.PositiveX:
          fromVec.x = 1;
          break;
        case constraint->constraint.aim.NegativeX:
          fromVec.x = -1;
          break;
        case constraint->constraint.aim.PositiveY:
          fromVec.y = 1;
          break;
        case constraint->constraint.aim.NegativeY:
          fromVec.y = -1;
          break;
        case constraint->constraint.aim.PositiveZ:
          fromVec.z = 1;
          break;
        case constraint->constraint.aim.NegativeZ:
          fromVec.z = -1;
          break;
        default:
          break;
        }

        glm::vec3 _trans, _scale, _skew;
        glm::vec4 _perspective;
        glm::quat rotationQuat;
        glm::decompose(worldMat, _scale, rotationQuat, _trans, _skew, _perspective);
        rotationQuat *= v4quat(node.rotation);
        fromVec = glm::mat4_cast(rotationQuat) * fromVec;
        glm::vec3 toVec = vmodel->physics.nodeMats[_src] * glm::vec4(0, 0, 0, 1) - vmodel->physics.nodeMats[i] * glm::vec4(0, 0, 0, 1);
        if (glm::length(toVec) != 0)
          toVec = glm::normalize(toVec);

        glm::quat fromToQuat = glm::rotation(glm::vec3(fromVec), toVec);

        glm::quat res = glm::mix(v4quat(node.rotation),
                                 glm::quat_cast(glm::inverse(worldMat) * glm::mat4_cast(fromToQuat) * worldMat) * v4quat(node.rotation),
                                 constraint->constraint.aim.weight);
        vmodel->physics.nodeTRS[i].rotation = glm::vec4(res.x, res.y, res.z, res.w);
      }
      else if (constraint->constraint.roll.source != -1)
      {
        uint _src = constraint->constraint.roll.source;

        glm::quat deltaSrcQuat = glm::inverse(v4quat(vmodel->model.nodes[_src].rotation)) * glm::quat(vmodel->physics.nodeTRS[_src].rotation.w, vmodel->physics.nodeTRS[_src].rotation.x, vmodel->physics.nodeTRS[_src].rotation.y, vmodel->physics.nodeTRS[_src].rotation.z);
        glm::quat deltaSrcQuatInParent = v4quat(vmodel->model.nodes[_src].rotation) * deltaSrcQuat * glm::inverse(v4quat(vmodel->model.nodes[_src].rotation));
        glm::quat deltaSrcQuatInDst = glm::inverse(v4quat(node.rotation)) * deltaSrcQuatInParent * v4quat(vmodel->model.nodes[_src].rotation);

        glm::vec3 rollAxis = glm::vec4(0);
        switch (constraint->constraint.roll.rollAxis)
        {
        case constraint->constraint.roll.X:
          rollAxis.x = 1;
          break;
        case constraint->constraint.roll.Y:
          rollAxis.y = 1;
          break;
        case constraint->constraint.roll.Z:
          rollAxis.z = 1;
          break;
        default:
          break;
        }
        glm::vec3 toVec = glm::mat4_cast(deltaSrcQuatInDst) * glm::vec4(rollAxis, 1);

        glm::quat fromToQuat = glm::rotation(rollAxis, toVec);
        glm::quat res = glm::mix(
            v4quat(node.rotation),
            v4quat(node.rotation) * glm::inverse(fromToQuat) * deltaSrcQuatInDst,
            constraint->constraint.roll.weight);
        vmodel->physics.nodeTRS[i].rotation = glm::vec4(res.x, res.y, res.z, res.w);
      }
      else if (constraint->constraint.rotation.source != -1)
      {
        uint _src = constraint->constraint.rotation.source;

        glm::quat srcDeltaQuat = glm::inverse(v4quat(vmodel->model.nodes[_src].rotation)) * glm::quat(vmodel->physics.nodeTRS[_src].rotation.w, vmodel->physics.nodeTRS[_src].rotation.x, vmodel->physics.nodeTRS[_src].rotation.y, vmodel->physics.nodeTRS[_src].rotation.z);
        glm::quat res = glm::mix(
            v4quat(node.rotation),
            v4quat(node.rotation) * srcDeltaQuat,
            constraint->constraint.rotation.weight);
        vmodel->physics.nodeTRS[i].rotation = glm::vec4(res.x, res.y, res.z, res.w);
      }
    }
  }
  return 0;
}

static int vrmPhysics(VModel_t *vmodel)
{
  vrmConstraints(vmodel);
  // gltf::Node &leftHand = vmodel->model.nodes[vmodelGetVRMNode(vmodel, "leftHand")];
  // gltf::Node& rightHand = vmodel->model.nodes[vmodelGetVRMNode(vmodel,"rightHand")];
  vmodel->physics.nodeTRS[vmodelGetVRMNode(vmodel, "leftHand")].translate.z += 0.001;
  // vmodel->physics.nodeTRS[vmodelGetVRMNode(vmodel, "leftUpperArm")].translate.z += 0.001;
  // vmodel->physics.nodeTRS[vmodelGetVRMNode(vmodel, "leftHand")].rotation = glm::normalize(vmodel->physics.nodeTRS[vmodelGetVRMNode(vmodel, "leftHand")].rotation);

  return 0;
}

int updateVModelPhysics(VModel_t *vmodel)
{
  if (ch_hashget(void *, vmodel->model.extensions, gltf::SUPPORTED_EXTENSIONS.VRMC_springBone))
  {
    return vrmPhysics(vmodel);
  }
  return 0;
}