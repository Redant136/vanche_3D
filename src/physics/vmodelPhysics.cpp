#include "vmodelPhysics.hpp"
#include "../loader/vanche_model.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#define v3(V) (glm::vec3(V[0], V[1], V[2]))
#define v4(V) (glm::vec4(V[0], V[1], V[2], V[3]))
#define v4quat(V) (glm::quat(V[3], V[0], V[1], V[2]))

static glm::quat getRotation(const glm::mat4 &mat)
{
  glm::vec3 _trans, _scale, _skew;
  glm::vec4 _perspective;
  glm::quat rotationQuat;
  glm::decompose(mat, _scale, rotationQuat, _trans, _skew, _perspective);
  return glm::normalize(rotationQuat);
}

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

        glm::vec3 fromVec = glm::vec4(0);
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

        glm::quat rotationQuat = getRotation(worldMat);
        rotationQuat *= v4quat(node.rotation);
        fromVec = rotationQuat * fromVec;
        glm::vec3 toVec = vmodel->physics.nodeMats[_src] * glm::vec4(0, 0, 0, 1) - vmodel->physics.nodeMats[i] * glm::vec4(0, 0, 0, 1);
        if (glm::length(toVec) != 0)
          toVec = glm::normalize(toVec);

        glm::quat fromToQuat = glm::rotation(fromVec, toVec);

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

static glm::vec3 vrmCalculateColliderDirection(VModel_t *vmodel, glm::vec3 pos, gltf::Extensions::VRMC_springBone::Collider &collider)
{
  return glm::vec3(0,0,0);
}
static int vrmSpringBone(VModel_t *vmodel)
{
  gltf::Extensions::VRMC_springBone *springBone = ch_hashget(gltf::Extensions::VRMC_springBone *, vmodel->model.extensions, gltf::SUPPORTED_EXTENSIONS.VRMC_springBone);
  if (springBone)
  {
    uint *nodeParents = (uint *)malloc(sizeof(uint) * vmodel->model.nodes.size());
    memset(nodeParents, -1, sizeof(uint) * vmodel->model.nodes.size());
    for (uint _spring = 0; _spring < springBone->springs.size(); _spring++)
    {
      gltf::Extensions::VRMC_springBone::Spring &spring = springBone->springs[_spring];

      for (uint i = 0; i < spring.joints.size(); i++)
      {
        for (uint j = 0; j < vmodel->model.nodes[spring.joints[i].node].children.size(); j++)
        {
          nodeParents[vmodel->model.nodes[spring.joints[i].node].children[j]] = spring.joints[i].node;
        }
        if (i == 0)
          continue;
        
        glm::mat4 center;
        if (spring.center == -1)
        {
          center=glm::mat4(1);
        }
        else
        {
          center = vmodel->physics.prevNodeMats[spring.center];
        }
        glm::vec3 prevTail = glm::inverse(center) * vmodel->physics.prevNodeMats[spring.joints[i].node] * glm::vec4(0, 0, 0, 1);
        glm::vec3 currentTail = glm::inverse(center) * vmodel->physics.nodeMats[spring.joints[i].node] * glm::vec4(0, 0, 0, 1);
        glm::vec3 initialBone = glm::inverse(center) * vmodel->physics.initialNodeMats[spring.joints[i].node] * glm::vec4(0, 0, 0, 1);
        glm::mat4 parentBone = nodeParents[spring.joints[i].node] == -1 ? glm::mat4(1) : 
          glm::inverse(center) * vmodel->physics.nodeMats[nodeParents[spring.joints[i].node]];
        glm::vec3 parentInitialBone = nodeParents[spring.joints[i].node] == -1 ? glm::vec3(0,0,0) : 
          glm::inverse(center) * vmodel->physics.initialNodeMats[nodeParents[spring.joints[i].node]] * glm::vec4(0, 0, 0, 1);
        glm::mat4 worldTransform = glm::inverse(vmodel->physics.nodeMats[spring.center]) * vmodel->physics.nodeMats[spring.joints[i].node] *
                                   glm::inverse(getNodeTransform(vmodel, spring.joints[i].node, glm::mat4(1)));

        float boneLength = glm::length(initialBone - parentInitialBone);
        glm::vec3 boneAxis = getNodeInitialTransform(vmodel, spring.joints[i].node, glm::mat4(1)) * glm::vec4(0, 0, 0, 1);
        boneAxis = glm::normalize(boneAxis);

        glm::vec3 inertia = (currentTail - prevTail) * (1 - spring.joints[i].dragForce);
        glm::vec3 stiffness = getRotation(parentBone) * v4quat(vmodel->model.nodes[spring.joints[i].node].rotation) * boneAxis;
        stiffness *= vmodel->physics.deltaTime * spring.joints[i].stiffness;
        glm::vec3 external = v3(spring.joints[i].gravityDir);
        external *= vmodel->physics.deltaTime * spring.joints[i].gravityPower;

        glm::vec3 nextTail = currentTail + inertia + stiffness*1.f + external;

        if (glm::any(glm::isnan(nextTail)))
        {
          fprintf(stderr, "Error in renderer. Number is NaN");
          return -1;
        }
        nextTail = glm::normalize(glm::vec3(glm::inverse(worldTransform) * glm::vec4(nextTail, 1))) * boneLength;


        // collisions
        for (uint colliderGroup = 0; colliderGroup < spring.colliderGroups.size(); colliderGroup++)
        {
          for (uint i = 0; i < springBone->colliderGroups[spring.colliderGroups[colliderGroup]].colliders.size(); i++)
          {
            nextTail = worldTransform * glm::vec4(nextTail, 1);
            nextTail -= vrmCalculateColliderDirection(vmodel, nextTail,
              springBone->colliders[springBone->colliderGroups[spring.colliderGroups[colliderGroup]].colliders[i]]);
            nextTail = glm::normalize(glm::vec3(glm::inverse(worldTransform) * glm::vec4(nextTail, 1))) * boneLength;
          }
        }
        vmodel->physics.nodeTRS[spring.joints[i].node].translate = nextTail;
      }
    }
    free(nodeParents);
  }
  // exit(0);
  return 0;
}
static int vrmCustomPhysics(VModel_t *vmodel)
{
  gltf::Extensions::VRMC_vrm *vrm = ch_hashget(gltf::Extensions::VRMC_vrm *, vmodel->model.extensions, gltf::SUPPORTED_EXTENSIONS.VRMC_vrm);
  if (vrm)
  {
  }
  return 0;
}

static int vrmPhysics(VModel_t *vmodel)
{
  chfpass(vrmConstraints, vmodel);
  chfpass(vrmSpringBone, vmodel);
  chfpass(vrmCustomPhysics, vmodel);
  // gltf::Node &leftHand = vmodel->model.nodes[vmodelGetVRMNode(vmodel, "leftHand")];
  // gltf::Node& rightHand = vmodel->model.nodes[vmodelGetVRMNode(vmodel,"rightHand")];
  // vmodel->physics.nodeTRS[42].translate.x -= 0.0001;
  // vmodel->physics.nodeTRS[vmodelGetVRMNode(vmodel, "leftHand")].translate.z += 0.001;
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
  memcpy(vmodel->physics.prevNodeTRS, vmodel->physics.nodeTRS, sizeof(VModel_t::VModelPhysics::NodeTRS) * vmodel->model.nodes.size());
  return 0;
}