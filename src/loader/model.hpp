#ifndef VANCHE_MODEL
#define VANCHE_MODEL
#include <vanche_utils.h>
#include <fstream>
#include <cstdint>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace gltf
{
#define GLTF_COMPONENT_BYTE (5120)
#define GLTF_COMPONENT_UBYTE (5121)
#define GLTF_COMPONENT_SHORT (5122)
#define GLTF_COMPONENT_USHORT (5123)
#define GLTF_COMPONENT_INT (5124)
#define GLTF_COMPONENT_UINT (5125)
#define GLTF_COMPONENT_FLOAT (5126)
#define GLTF_COMPONENT_DOUBLE (5130)
  typedef std::string string;

  static const struct
  {
    const string KHR_materials_unlit = "KHR_materials_unlit";
    const string KHR_texture_transform = "KHR_texture_transform";
    const string KHR_materials_emissive_strength = "KHR_materials_emissive_strength";
    const string VRM = "VRM";
    const string VRMC_springBone = "VRMC_springBone";
    const string VRMC_materials_mtoon = "VRMC_materials_mtoon";
    const string VRMC_node_constraint = "VRMC_node_constraint";
    const string VRMC_vrm = "VRMC_vrm";
  } SUPPORTED_EXTENSIONS;

  struct Extension
  {
    string name;
    void *data;
  };

  struct Buffer
  {
    uchar *buffer = NULL;
    //----------------
    string uri = "";
    int byteLength = 0;
    string name = "";
    std::vector<Extension> extensions;
    std::vector<Extension> extras;
  };
  struct BufferView
  {
    int buffer = 0;
    int byteOffset = 0;
    int byteLength = 0;
    int byteStride = 0;
    int target = 34962;
    string name = "";
    std::vector<Extension> extensions;
    std::vector<Extension> extras;
  };
  struct Accessor
  {
    int bufferView = -1;
    int byteOffset = 0;
    enum Types
    {
      SCALAR,
      VEC2,
      VEC3,
      VEC4,
      MAT2,
      MAT3,
      MAT4
    } type;
    enum glComponentType
    {
      BYTE = GLTF_COMPONENT_BYTE,
      UNSIGNED_BYTE = GLTF_COMPONENT_UBYTE,
      SHORT = GLTF_COMPONENT_SHORT,
      UNSIGNED_SHORT = GLTF_COMPONENT_USHORT,
      INT = GLTF_COMPONENT_INT,
      UNSIGNED_INT = GLTF_COMPONENT_UINT,
      FLOAT = GLTF_COMPONENT_FLOAT,
      DOUBLE = GLTF_COMPONENT_DOUBLE
    } componentType = FLOAT;
    int count = 0;
    std::vector<float> max;
    std::vector<float> min;
    bool normalized = false;
    struct Sparse
    {
      uint count = 0;
      struct Indices
      {
        int bufferView = -1;
        int byteOffset = 0;
        enum glComponentType
        {
          UNSIGNED_BYTE = GLTF_COMPONENT_UBYTE,
          UNSIGNED_SHORT = GLTF_COMPONENT_USHORT,
          UNSIGNED_INT = GLTF_COMPONENT_UINT
        } componentType;
        std::vector<Extension> extensions;
        std::vector<Extension> extras;
      } indices;
      struct Values
      {
        int bufferView = -1;
        int byteOffset = 0;
        std::vector<Extension> extensions;
        std::vector<Extension> extras;
      } values;
      std::vector<Extension> extensions;
      std::vector<Extension> extras;
    } sparse;
    string name = "";
    std::vector<Extension> extensions;
    std::vector<Extension> extras;
  };
  //-----------------
  struct Image
  {
    string name = "";
    string uri = "";
    int bufferView = -1;
    string mimeType = "";
    std::vector<Extension> extensions;
    std::vector<Extension> extras;
  };
  struct Sampler
  {
    enum glFilter
    {
      NONE = 0,
      NEAREST = GL_NEAREST,
      LINEAR = GL_LINEAR,
      NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
      LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
      NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
      LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
    };
    enum glWrap
    {
      CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
      CLAMP_BORDER = GL_CLAMP_TO_BORDER,
      MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
      REPEAT = GL_REPEAT
    };
    glFilter magFilter = NEAREST;
    glFilter minFilter = NEAREST;
    glWrap wrapS = CLAMP_TO_EDGE;
    glWrap wrapT = CLAMP_TO_EDGE;
    string name = "";
    std::vector<Extension> extensions;
    std::vector<Extension> extras;
  };
  struct Texture
  {
    int sampler = -1;
    int source = -1;
    string name = "";
    std::vector<Extension> extensions;
    std::vector<Extension> extras;
  };
  struct Material
  {
    struct TextureInfo
    {
      int index = -1;
      int texCoord = 0;
      std::vector<Extension> extensions;
      std::vector<Extension> extras;
    };
    struct PbrMetallicRoughness
    {
      TextureInfo baseColorTexture;
      std::vector<float> baseColorFactor = {1.f, 1.f, 1.f, 1.f};
      TextureInfo metallicRoughnessTexture;
      float metallicFactor = 1.f;
      float roughnessFactor = 1.f;
      std::vector<Extension> extensions;
      std::vector<Extension> extras;
    };
    struct MaterialNormalTextureInfo : public TextureInfo
    {
      float scale = 1.f;
    };
    struct MaterialOcclusionTextureInfo : public TextureInfo
    {
      float strength = 1.f;
    };
    string name = "";
    PbrMetallicRoughness pbrMetallicRoughness;
    MaterialNormalTextureInfo normalTexture;
    MaterialOcclusionTextureInfo occlusionTexture;
    TextureInfo emissiveTexture;
    std::vector<float> emissiveFactor;
    enum AlphaMode
    {
      OPAQUE = 0,
      MASK,
      BLEND
    } alphaMode = OPAQUE;
    float alphaCutoff = 0.f;
    bool doubleSided = false;
    std::vector<Extension> extensions;
    std::vector<Extension> extras;
  };
  //-------
  struct Camera
  {
    struct Orthographic
    {
      float xmag = 0.f;
      float ymag = 0.f;
      float zfar = 0.f;
      float znear = 0.f;
      std::vector<Extension> extensions;
      std::vector<Extension> extras;
    };
    struct Perspective
    {
      float aspectRatio = 1.f;
      float yfov = 1.f;
      float zfar = 0.f;
      float znear = 0.f;
      std::vector<Extension> extensions;
      std::vector<Extension> extras;
    };
    string name = "";
    Orthographic orthographic;
    Perspective perspective;
    enum ProjectionType
    {
      Perspective,
      Orthographic
    } type;
    std::vector<Extension> extensions;
    std::vector<Extension> extras;
  };
  //--------
  struct Skin
  {
    string name = "";
    int inverseBindMatrices = -1;
    std::vector<int> joints;
    int skeleton = -1;
    std::vector<Extension> extensions;
    std::vector<Extension> extras;
  };
  //--------
  struct Animation
  {
    struct AnimationChannel
    {
      int sampler = -1;
      struct AnimationTarget
      {
        int node = -1;
        enum
        {
          translation,
          rotation,
          scale,
          weights
        } path;
        std::vector<Extension> extensions;
        std::vector<Extension> extras;
      } target;
      std::vector<Extension> extensions;
      std::vector<Extension> extras;
    };
    struct AnimationSampler
    {
      int input = -1;
      enum
      {
        LINEAR,
        STEP,
        CUBICSPLINE
      } interpolation;
      int output = -1;
      std::vector<Extension> extensions;
      std::vector<Extension> extras;
    };
    string name = "";
    std::vector<AnimationChannel> channels;
    std::vector<AnimationSampler> samplers;
    std::vector<Extension> extensions;
    std::vector<Extension> extras;
  };
  //--------
  struct Mesh
  {
    struct Primitive
    {
      struct MorphTarget
      {
        int POSITION = -1;
        int NORMAL = -1;
        int TANGENT = -1;
      };
      int mode = 4;
      int indices = -1;
      struct Attributes
      {
        int POSITION = -1;
        int NORMAL = -1;
        int TANGENT = -1;
        int TEXCOORD_0 = -1;
        int TEXCOORD_1 = -1;
        int TEXCOORD_2 = -1;
        int COLOR_0 = -1;
        int JOINTS_0 = -1;
        int WEIGHTS_0 = -1;
      } attributes;
      int material = -1;
      std::vector<MorphTarget> targets;
      std::vector<Extension> extensions;
      std::vector<Extension> extras;
    };
    std::string name = "";
    std::vector<Primitive> primitives;
    std::vector<float> weights;
    std::vector<Extension> extensions;
    std::vector<Extension> extras;
  };
  struct Node
  {
    string name = "";
    std::vector<uint> children = std::vector<uint>();
    float matrix[16] = {1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        0, 0, 0, 1};
    float translation[3] = {0, 0, 0};
    float rotation[4] = {0, 0, 0, 1};
    float scale[3] = {1, 1, 1};
    int mesh = -1;
    int skin = -1;
    std::vector<float> weights = std::vector<float>();
    int camera = -1;
    std::vector<Extension> extensions = std::vector<Extension>();
    std::vector<Extension> extras = std::vector<Extension>();
  };

  struct Scene
  {
    string name = "";
    std::vector<int> nodes;
    std::vector<Extension> extensions;
    std::vector<Extension> extras;
  };

  struct glTFModel
  {
    struct
    {
      string version;
      string generator;
      string copyright;
      string minVersion;
      //---------------------------------
      std::vector<Extension> extensions;
      std::vector<Extension> extras;
    } asset;
    std::vector<Buffer> buffers;
    std::vector<BufferView> bufferViews;

    std::vector<Accessor> accessors;
    std::vector<Texture> textures;
    std::vector<Sampler> samplers;
    std::vector<Image> images;
    std::vector<Material> materials;
    std::vector<Mesh> meshes;

    std::vector<Node> nodes;
    std::vector<Skin> skins;

    int scene = -1;
    std::vector<Scene> scenes;

    std::vector<Animation> animations;
    std::vector<Camera> cameras;
    std::vector<string> extensionsUsed;
    std::vector<string> extensionsRequired;

    std::vector<Extension> extensions;
    std::vector<Extension> extras;
  };

  namespace Extensions
  {
    struct KHR_materials_unlit
    {
      std::vector<Extension> extensions;
      std::vector<Extension> extras;
    };
    struct KHR_texture_transform
    {
      float offset[2] = {0, 0};
      float rotation = 0.0;
      float scale[2] = {1, 1};
      int texCoord = 0;
      std::vector<Extension> extensions;
      std::vector<Extension> extras;
    };
    struct KHR_materials_emissive_strength
    {
      float emissiveStrength = 1;
    };
    struct VRM
    {
      union Vec3
      {
        glm::vec3 glm_vec;
        struct
        {
          float x, y, z;
        };
      };
      union Vec4
      {
        glm::vec4 glm_vec;
        struct
        {
          float x, y, z, w;
        };
      };
      VRM() = default;
      string exporterVersion;
      string specVersion;
      struct Meta
      {
        string title;
        string version;
        string author;
        string contactInformation;
        string reference;
        int texture;
        enum
        {
          OnlyAuthor,
          ExplicitlyLicensedPerson,
          Everyone
        } allowedUserName;
        string violentUssageName;
        string sexualUssageName;
        string commercialUssageName;
        string otherPermissionUrl;
        enum
        {
          Redistribution_Prohibited,
          CC0,
          CC_BY,
          CC_BY_NC,
          CC_BY_SA,
          CC_BY_NC_SA,
          CC_BY_ND,
          CC_BY_NC_ND,
          Other
        } licenseName;
        string otherLicenseUrl;
      } meta;
      struct Humanoid
      {
        struct Bone
        {
          enum
          {
            hips,
            leftUpperLeg,
            rightUpperLeg,
            leftLowerLeg,
            rightLowerLeg,
            leftFoot,
            rightFoot,
            spine,
            chest,
            neck,
            head,
            leftShoulder,
            rightShoulder,
            leftUpperArm,
            rightUpperArm,
            leftLowerArm,
            rightLowerArm,
            leftHand,
            rightHand,
            leftToes,
            rightToes,
            leftEye,
            rightEye,
            jaw,
            leftThumbProximal,
            leftThumbIntermediate,
            leftThumbDistal,
            leftIndexProximal,
            leftIndexIntermediate,
            leftIndexDistal,
            leftMiddleProximal,
            leftMiddleIntermediate,
            leftMiddleDistal,
            leftRingProximal,
            leftRingIntermediate,
            leftRingDistal,
            leftLittleProximal,
            leftLittleIntermediate,
            leftLittleDistal,
            rightThumbProximal,
            rightThumbIntermediate,
            rightThumbDistal,
            rightIndexProximal,
            rightIndexIntermediate,
            rightIndexDistal,
            rightMiddleProximal,
            rightMiddleIntermediate,
            rightMiddleDistal,
            rightRingProximal,
            rightRingIntermediate,
            rightRingDistal,
            rightLittleProximal,
            rightLittleIntermediate,
            rightLittleDistal,
            upperChest
          } bone;
          int node;
          bool useDefaultValues;
          Vec3 min, max, center;
          float axisLength; // figure out what this does
        };
        std::vector<Bone> humanBones;
        float armStretch, legStretch, upperArmTwist, lowerArmTwist, upperLegTwist, lowerLegTwist, feetSpacing; // inverse kinematic bounds
        bool hasTranslationDoF;
      } humanoid;
      struct FirstPerson
      {
        struct MeshAnnotations
        {
          int mesh = -1;
          string type = "";
        };
        struct DegreeMap
        {
          std::vector<float> curve;
          float xRange;
          float yRange;
        };
        int firstPersonBone;
        Vec3 firstPersonBoneOffset;
        std::vector<MeshAnnotations> meshAnnotations;
        enum
        {
          Bone,
          BlendShape
        } lookAtTypeName;
        DegreeMap lookAtHorizontalInner;
        DegreeMap lookAtHorizontalOuter;
        DegreeMap lookAtVerticalDown;
        DegreeMap lookAtVerticalUp;
      } firstPerson;
      struct BlendShapeMaster
      {
        struct BlendShapeGroup
        {
          struct Bind
          {
            uint mesh;
            uint index;
            float weight;
          };
          struct MaterialBind
          {
            string type;
            string propertyName;
            std::vector<float> targetValue;
          };
          string name = "";
          enum PresetNames
          {
            unknown,
            neutral,
            a,
            i,
            u,
            e,
            o,
            blink,
            joy,
            angry,
            sorrow,
            fun,
            lookup,
            lookdown,
            lookleft,
            lookright,
            blink_l,
            blink_r
          } presetName;
          std::vector<Bind> binds;
          std::vector<MaterialBind> materialValues;
          bool isBinary;
        };
        std::vector<BlendShapeGroup> blendShapeGroups;
      } blendShapeMaster;
      struct SecondaryAnimation
      {
        struct Spring
        {
          string comment;
          float stiffiness;
          float gravityPower;
          Vec3 gravityDir;
          float dragForce;
          int center;
          float hitRadius;
          std::vector<uint> bones;
          std::vector<int> colliderGroups;
        };
        struct ColliderGroup
        {
          struct Collider
          {
            Vec3 offset;
            float radius;
          };
          int node;
          std::vector<Collider> colliders;
        };
        std::vector<Spring> boneGroups;
        std::vector<ColliderGroup> colliderGroups;
      } secondaryAnimation;
      struct MaterialProperties
      {
        string name;
        string shader = "VRM/MToon";
        int renderQueue;
        struct FloatProperties
        {
          float _Cutoff, _BumpScale, _ReceiveShadowRate, _ShadingGradeRate, _ShadeShift, _ShadeToony, _LightColorAttenuation, _IndirectLightIntensity, _RimLightingMix, _RimFresnelPower, _RimLift, _OutlineWidth, _OutlineScaledMaxDistance, _OutlineLightingMix, _DebugMode, _BlendMode, _OutlineWidthMode, _OutlineColorMode, _CullMode, _OutlineCullMode, _SrcBlend, _DstBlend, _ZWrite;
        } floatProperties;
        struct VectorProperties
        {
          Vec4 _Color, _ShadeColor, _MainTex, _MainTex_ST, _ShadeTexture, _BumpMap, _ReceiveShadowTexture, _ShadingGradeTexture, _SphereAdd, _RimColor, _EmissionColor, _EmissionMap, _OutlineWidthTexture, _OutlineColor, _UvAnimMaskTexture;
        } vectorProperties;
        struct
        {
          int _MainTex, _ShadeTexture, _BumpMap, _ReceiveShadowTexture, _ShadingGradeTexture, _RimTexture, _SphereAdd, _EmissionMap, _OutlineWidthTexture, _UvAnimMaskTexture;
        } textureProperties;
        struct
        {
          bool MTOON_CLIP_IF_OUTLINE_IS_NONE, MTOON_OUTLINE_WIDTH_WORLD, MTOON_OUTLINE_WIDTH_SCREEN, _ALPHATEST_ON, _ALPHABLEND_ON, _NORMALMAP, MTOON_FORWARD_ADD, MTOON_OUTLINE_COLOR_FIXED, MTOON_OUTLINE_COLOR_MIXED, MTOON_DEBUG_NORMAL, MTOON_DEBUG_LITSHADERATE, MTOON_OUTLINE_COLORED;
        } keywordMap;
        struct
        {
          string RenderType;
        } tagMap;
      };
      std::vector<MaterialProperties> materialProperties;
    };
    struct VRMC_springBone
    {
      struct Collider
      {
        int node = -1;
        struct
        {
          bool isSphere = 1;
          struct
          {
            float offset[3] = {0, 0, 0};
            float radius = 0;
          } sphere;
          struct
          {
            float offset[3] = {0, 0, 0};
            float radius = 0;
            float tail[3] = {0, 0, 0};
          } capsule;
        } shape;
      };
      struct ColliderGroup
      {
        string name;
        std::vector<int> colliders;
      };
      struct Spring
      {
        struct Joint
        {
          int node = -1;
          float hitRadius = 0;
          float stiffness = 1;
          float gravityPower = 0;
          float gravityDir[3] = {0, -1, 0};
          float dragForce = 0.5;
        };
        string name;
        std::vector<Joint> joints;
        std::vector<int> colliderGroups;
        int center = -1;
      };
      string specVersion;
      std::vector<Collider> colliders;
      std::vector<ColliderGroup> colliderGroups;
      std::vector<Spring> springs;
    };
    struct VRMC_materials_mtoon
    {
      string specVersion;
      bool transparentWithZWrite = false;
      int renderQueueOffsetNumber;
      float shadeColorFactor[3] = {1, 1, 1};
      Material::TextureInfo shadeMultiplyTexture;
      float shadingShiftFactor = 0;
      struct ShadingShiftTexture : public Material::TextureInfo
      {
        float scale = 1;
      } shadingShiftTexture;
      float shadingToonyFactor = 0.9;
      float giEqualizationFactor = 0.9;
      float matcapFactor[3] = {0, 0, 0};
      Material::TextureInfo matcapTexture;
      float parametricRimColorFactor[3] = {0, 0, 0};
      Material::TextureInfo rimMultiplyTexture;
      float rimLightingMixFactor = 1;
      float parametricRimFresnelPowerFactor = 5;
      float parametricRimLiftFactor = 0;
      enum OutlineWidthMode
      {
        none = 0,
        worldCoordinates = 1,
        screenCoordinates = 2
      } outlineWidthMode = none;
      float outlineWidthFactor;
      Material::TextureInfo outlineWidthMultiplyTexture;
      float outlineColorFactor[3] = {0, 0, 0};
      float outlineLightingMixFactor = 1;
      Material::TextureInfo uvAnimationMaskTexture;
      float uvAnimationScrollXSpeedFactor = 0;
      float uvAnimationScrollYSpeedFactor = 0;
      float uvAnimationRotationSpeedFactor = 0;
    };
    struct VRMC_node_constraint
    {
      string specVersion;
      struct
      {
        struct
        {
          int source = -1;
          enum
          {
            X,
            Y,
            Z
          } rollAxis;
          float weight = 1;
        } roll;
        struct
        {
          int source = -1;
          enum
          {
            PositiveX,
            NegativeX,
            PositiveY,
            NegativeY,
            PositiveZ,
            NegativeZ
          } aimAxis;
          float weight = 1;
        } aim;
        struct
        {
          int source = -1;
          float weight = 1;
        } rotation;
      } constraint;
    };
    struct VRMC_vrm
    {
      string specVersion;
      struct Meta
      {
        string name;
        string version;
        std::vector<string> authors;
        string copyrightInformation;
        string contactInformation;
        std::vector<string> reference;
        string thirdPartyLicenses;
        int thumbnailImage = -1;
        string licenseUrl;
        enum
        {
          onlyAuthor,
          onlySeparatelyLicensedPerson,
          everyone
        } avatarPermission = onlyAuthor;
        bool allowExcessivelyViolentUsage = false;
        bool allowExcessivelySexualUsage = false;
        enum
        {
          personalNonProfit,
          personalProfit,
          corporation
        } commercialUsage = personalNonProfit;
        bool allowPoliticalOrReligiousUsage = false;
        bool allowAntisocialOrHateUsage = false;
        enum
        {
          required,
          unnecessary
        } creditNotation = required;
        bool allowRedistribution = false;
        enum
        {
          prohibited,
          allowModification,
          allowModificationRedistribution
        } modification = prohibited;
        string otherLicenseUrl;
      } meta;
      struct Humanoid
      {
        struct HumanBones
        {
          struct Bone
          {
            int node = -1;
          } hips,
              spine,
              chest,
              upperChest,
              neck,
              head,
              leftEye,
              rightEye,
              jaw,
              leftUpperLeg,
              leftLowerLeg,
              leftFoot,
              leftToes,
              rightUpperLeg,
              rightLowerLeg,
              rightFoot,
              rightToes,
              leftShoulder,
              leftUpperArm,
              leftLowerArm,
              leftHand,
              rightShoulder,
              rightUpperArm,
              rightLowerArm,
              rightHand,
              leftThumbMetacarpal,
              leftThumbProximal,
              leftThumbDistal,
              leftIndexProximal,
              leftIndexIntermediate,
              leftIndexDistal,
              leftMiddleProximal,
              leftMiddleIntermediate,
              leftMiddleDistal,
              leftRingProximal,
              leftRingIntermediate,
              leftRingDistal,
              leftLittleProximal,
              leftLittleIntermediate,
              leftLittleDistal,
              rightThumbMetacarpal,
              rightThumbProximal,
              rightThumbDistal,
              rightIndexProximal,
              rightIndexIntermediate,
              rightIndexDistal,
              rightMiddleProximal,
              rightMiddleIntermediate,
              rightMiddleDistal,
              rightRingProximal,
              rightRingIntermediate,
              rightRingDistal,
              rightLittleProximal,
              rightLittleIntermediate,
              rightLittleDistal;
        } humanBones;
      } humanoid;
      struct FirstPerson
      {
        struct MeshAnnotations
        {
          int node = -1;
          enum
          {
            Auto,
            both,
            thirdPersonOnly,
            firstPersonOnly
          } type;
        };
        std::vector<MeshAnnotations> meshAnnotations;
      } firstPerson;
      struct LookAt
      {
        float offsetFromHeadBone[3] = {0, 0, 0};
        enum
        {
          bone,
          expression
        } type;
        struct RangeMap
        {
          float inputMaxValue, outputScale;
        };
        RangeMap rangeMapHorizontalInner;
        RangeMap rangeMapHorizontalOuter;
        RangeMap rangeMapVerticalDown;
        RangeMap rangeMapVerticalUp;
      } lookAt;
      struct ExpressionPresets
      {
        struct Expression
        {
          struct MorphTargetBind
          {
            int node = -1;
            int index = -1;
            float weight;
          };
          struct MaterialColorBind
          {
            int material;
            enum
            {
              color,
              emissionColor,
              shadeColor,
              matcapColor,
              rimColor,
              outlineColor
            } type;
            float targetValue[4];
          };
          struct TextureTransformBind
          {
            int material = -1;
            float scale[2] = {1, 1};
            float offset[2] = {0, 0};
          };
          std::vector<MorphTargetBind> morphTargetBinds;
          std::vector<MaterialColorBind> materialColorBinds;
          std::vector<TextureTransformBind> textureTransformBinds;
          bool isBinary = false;
          enum BlockBlend
          {
            none,
            block,
            blend
          };
          BlockBlend overrideBlink = none;
          BlockBlend overrideLookAt = none;
          BlockBlend overrideMouth = none;
        };
        struct Preset
        {
          Expression happy,
              angry,
              sad,
              relaxed,
              surprised,
              aa,
              ih,
              ou,
              ee,
              oh,
              blink,
              blinkLeft,
              blinkRight,
              lookUp,
              lookDown,
              lookLeft,
              lookRight,
              neutral;
        } preset;
        Expression custom;
      } expressions;
    };
  }

  namespace Extras{
    struct TargetNames{
      std::vector<std::string> targetNames;
    };
  }

  template<typename T>
  static int getMeshPrimitiveAttribVal(const T &attribute, std::string name)
  {
    int attrib = attribute.POSITION;
#define parseAttrib(att_name)    \
  if (name == #att_name)         \
  {                              \
    attrib = attribute.att_name; \
  }
    parseAttrib(POSITION) else parseAttrib(NORMAL) else parseAttrib(TANGENT) else parseAttrib(TEXCOORD_0) else parseAttrib(TEXCOORD_1) else parseAttrib(TEXCOORD_2) else parseAttrib(COLOR_0) else parseAttrib(JOINTS_0) else parseAttrib(WEIGHTS_0) else
    {
      assert(0);
    }
#undef parseAttrib
    return attrib;
  }

  static uint gltf_sizeof(int type)
  {
    switch (type)
    {
    case GLTF_COMPONENT_BYTE:
    case GLTF_COMPONENT_UBYTE:
      return sizeof(char);
    case GLTF_COMPONENT_SHORT:
    case GLTF_COMPONENT_USHORT:
      return sizeof(short);
    case GLTF_COMPONENT_INT:
    case GLTF_COMPONENT_UINT:
      return sizeof(int);
    case GLTF_COMPONENT_FLOAT:
      return sizeof(float);
    case GLTF_COMPONENT_DOUBLE:
      return sizeof(double);
    default:
      assert(0);
      return 0;
    }
  }

  static uint gltf_num_components(Accessor::Types type)
  {
    switch (type)
    {
    case Accessor::Types::SCALAR:
      return 1;
    case Accessor::Types::VEC2:
      return 2;
    case Accessor::Types::VEC3:
      return 3;
    case Accessor::Types::VEC4:
      return 4;
    case Accessor::Types::MAT2:
      return 4;
    case Accessor::Types::MAT3:
      return 9;
    case Accessor::Types::MAT4:
      return 16;
    default:
      return 0;
    }
  }

  static uchar *getDataFromAccessor(const glTFModel &model, const Accessor &accessor, uint index)
  {
    if(accessor.sparse.count>0){
      uchar *bfIndiceData = model.buffers[model.bufferViews[accessor.sparse.indices.bufferView].buffer].buffer;
      bfIndiceData += model.bufferViews[accessor.sparse.indices.bufferView].byteOffset;
      bfIndiceData += accessor.sparse.indices.byteOffset;
      for (uint l = 0; l < accessor.sparse.count; l++)
      {
        ulong sparseIndex=CHVAL;
        if (accessor.sparse.indices.componentType == gltf::Accessor::Sparse::Indices::UNSIGNED_BYTE)
          sparseIndex = *(((uint8_t *)bfIndiceData) + l);
        else if (accessor.sparse.indices.componentType == gltf::Accessor::Sparse::Indices::UNSIGNED_SHORT)
          sparseIndex = *(((uint16_t *)bfIndiceData) + l);
        else
          sparseIndex = *(((uint32_t *)bfIndiceData) + l);

        if(sparseIndex!=index)
          continue;
        uchar *bfValData = model.buffers[model.bufferViews[accessor.sparse.values.bufferView].buffer].buffer;
        bfValData += model.bufferViews[accessor.sparse.values.bufferView].byteOffset;
        bfValData += accessor.sparse.values.byteOffset;
        bfValData += gltf::gltf_num_components(accessor.type) * gltf::gltf_sizeof(accessor.componentType) * l;
        return bfValData;
      }
    }
    if (accessor.bufferView == -1)
      return 0;
    const BufferView &bfView = model.bufferViews[accessor.bufferView];
    uint byteStride = bfView.byteStride;
    if (byteStride == 0)
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
    assert(accessor.count > index);

    uchar *data = (model.buffers[bfView.buffer].buffer + bfView.byteOffset) + accessor.byteOffset + (byteStride)*index;
    // MAX-MIN
    if (0)
    {
      for (int i = 0; i < accessor.max.size(); i++)
      {
        if (accessor.componentType == GLTF_COMPONENT_BYTE)
        {
          membuild(char, a, data + i);
          a = MAX(accessor.max[i], MIN(accessor.min[i], a));
          memcpy(data + i, &a, sizeof(a));
        }
        else if (accessor.componentType == GLTF_COMPONENT_UBYTE)
        {
          membuild(uchar, a, data + i);
          a = MAX(accessor.max[i], MIN(accessor.min[i], a));
          memcpy(data + i, &a, sizeof(a));
        }
        else if (accessor.componentType == GLTF_COMPONENT_SHORT)
        {
          membuild(int16_t, a, data + i * sizeof(int16_t));
          a = MAX(accessor.max[i], MIN(accessor.min[i], a));
          memcpy(data + i * sizeof(int16_t), &a, sizeof(a));
        }
        else if (accessor.componentType == GLTF_COMPONENT_USHORT)
        {
          membuild(uint16_t, a, data + i * sizeof(uint16_t));
          a = MAX(accessor.max[i], MIN(accessor.min[i], a));
          memcpy(data + i * sizeof(uint16_t), &a, sizeof(a));
        }
        else if (accessor.componentType == GLTF_COMPONENT_INT)
        {
          membuild(int32_t, a, data + i * sizeof(int32_t));
          a = MAX(accessor.max[i], MIN(accessor.min[i], a));
          memcpy(data + i * sizeof(int32_t), &a, sizeof(a));
        }
        else if (accessor.componentType == GLTF_COMPONENT_UINT)
        {
          membuild(uint32_t, a, data + i * sizeof(uint32_t));
          a = MAX(accessor.max[i], MIN(accessor.min[i], a));
          memcpy(data + i * sizeof(uint32_t), &a, sizeof(a));
        }
        else if (accessor.componentType == GLTF_COMPONENT_FLOAT)
        {
          membuild(float, a, data + i * sizeof(float));
          a = MAX(accessor.max[i], MIN(accessor.min[i], a));
          memcpy(data + i * sizeof(float), &a, sizeof(a));
        }
        else if (accessor.componentType == GLTF_COMPONENT_DOUBLE)
        {
          membuild(double, a, data + i * sizeof(double));
          a = MAX(accessor.max[i], MIN(accessor.min[i], a));
          memcpy(data + i * sizeof(double), &a, sizeof(a));
        }
      }
    }
    return data;
  }

  template <typename T>
  static int findExtensionIndex(string name, T obj)
  {
    for (int i = 0; i < obj.extensions.size(); i++)
    {
      if (obj.extensions[i].name == name)
      {
        return i;
      }
    }
    return -1;
  }

  static void freeModel(glTFModel *model)
  {
    for (uint i = 0; i < model->buffers.size(); i++)
    {
      if (model->buffers[i].buffer)
      {
        delete[] model->buffers[i].buffer;
        model->buffers[i].buffer = 0;
      }
    }
    for (uint i = 0; i < model->extensions.size(); i++)
    {
      if(model->extensions[i].name=="KHR_materials_unlit"){
        delete (Extensions::KHR_materials_unlit*)model->extensions[i].data;
      }
      else if(model->extensions[i].name=="KHR_texture_transform"){
        delete (Extensions::KHR_texture_transform*)model->extensions[i].data;
      }
      else if(model->extensions[i].name=="KHR_materials_emissive_strength"){
        delete (Extensions::KHR_materials_emissive_strength*)model->extensions[i].data;
      }
      else if(model->extensions[i].name=="VRM"){
        delete (Extensions::VRM*)model->extensions[i].data;
      }
      else if(model->extensions[i].name=="VRMC_springBone"){
        delete (Extensions::VRMC_springBone*)model->extensions[i].data;
      }
      else if(model->extensions[i].name=="VRMC_materials_mtoon"){
        delete (Extensions::VRMC_materials_mtoon*)model->extensions[i].data;
      }
      else if(model->extensions[i].name=="VRMC_node_constraint"){
        delete (Extensions::VRMC_node_constraint*)model->extensions[i].data;
      }
      else if(model->extensions[i].name=="VRMC_vrm"){
        delete (Extensions::VRMC_vrm*)model->extensions[i].data;
      }
    }
  }
}

#endif