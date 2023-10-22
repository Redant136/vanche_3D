#version 330 core
#define MAX_JOINT_MATRIX 256
#define MAX_NODES 512

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec4 a_tangent;
layout (location = 3) in vec2 a_texCoords_0;
layout (location = 4) in vec2 a_texCoords_1;
layout (location = 5) in vec2 a_texCoords_2;
layout (location = 6) in vec4 a_color_0;
layout (location = 7) in vec4 a_joints;
layout (location = 8) in vec4 a_weights;
layout (location = 9) in vec3 a_posMorph;
layout (location = 10) in vec3 a_normMorph;
layout (location = 11) in vec3 a_tanMorph;


layout (std140) uniform Transforms{
    mat4 worldTransform;// camera and global model transform
    mat4 model; // global model transform
    int nodeIndex;// specific node transform index
    float _pad1,_pad2,_pad3; // for padding issues
    ivec4 jointNodes[MAX_JOINT_MATRIX>>2]; // array of joint nodes index, I hate padding
};

layout (std140) uniform Nodes{
    mat4 nodes[MAX_NODES]; // all node transforms in the model
};

layout (std140) uniform InverseBindMatrices{
    mat4 invBindMatrix[MAX_JOINT_MATRIX]; // inverse bind matrices
}; 

uniform int texCoordIndex;
uniform sampler2D texture_normal;
// KHR_texture_transform
uniform struct KHR_texture_transform_data_t{
  vec2 u_offset, u_scale;
  float u_rotation;
}KHR_texture_transform_data;


out VS_OUT{
    vec3 Pos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec4 Color;
    vec3 BitTangent;
} vs_out;


void main()
{
    if(texCoordIndex==0){
        vs_out.TexCoords=a_texCoords_0;
    }else if(texCoordIndex==1){
        vs_out.TexCoords=a_texCoords_1;
    }else if(texCoordIndex==2){
        vs_out.TexCoords=a_texCoords_2;
    }
    vec2 UV=vs_out.TexCoords;
    UV = (
        mat3(1,0,0, 0,1,0, KHR_texture_transform_data.u_offset.x, KHR_texture_transform_data.u_offset.y, 1)*
        mat3( cos(KHR_texture_transform_data.u_rotation), sin(KHR_texture_transform_data.u_rotation), 0,
                -sin(KHR_texture_transform_data.u_rotation), cos(KHR_texture_transform_data.u_rotation), 0,
                0,             0, 1)*
        mat3(KHR_texture_transform_data.u_scale.x,0,0, 0,KHR_texture_transform_data.u_scale.y,0, 0,0,1)*
        vec3(vs_out.TexCoords,1)).xy;
    vs_out.TexCoords=UV;

    mat4 skinMatrix = mat4(0.f);
    mat4 invNode = inverse(nodes[nodeIndex]);
    if(a_joints.x<(jointNodes.length()<<2)&&a_joints.x>-1){
        int i=int(a_joints.x);
        i=(i&3)==0?jointNodes[i>>2].x:((i&3)==1?jointNodes[i>>2].y:((i&3)==2?jointNodes[i>>2].z:jointNodes[i>>2].w));
        skinMatrix+=a_weights.x*invNode*nodes[i]*invBindMatrix[int(a_joints.x)];
    }
    if(a_joints.y<(jointNodes.length()<<2)&&a_joints.y>-1){
        int i=int(a_joints.y);
        i=(i&3)==0?jointNodes[i>>2].x:((i&3)==1?jointNodes[i>>2].y:((i&3)==2?jointNodes[i>>2].z:jointNodes[i>>2].w));
        skinMatrix+=a_weights.y*invNode*nodes[i]*invBindMatrix[int(a_joints.y)];
    }
    if(a_joints.z<(jointNodes.length()<<2)&&a_joints.z>-1){
        int i=int(a_joints.z);
        i=(i&3)==0?jointNodes[i>>2].x:((i&3)==1?jointNodes[i>>2].y:((i&3)==2?jointNodes[i>>2].z:jointNodes[i>>2].w));
        skinMatrix+=a_weights.z*invNode*nodes[i]*invBindMatrix[int(a_joints.z)];
    }
    if(a_joints.w<(jointNodes.length()<<2)&&a_joints.w>-1){
        int i=int(a_joints.w);
        i=(i&3)==0?jointNodes[i>>2].x:((i&3)==1?jointNodes[i>>2].y:((i&3)==2?jointNodes[i>>2].z:jointNodes[i>>2].w));
        skinMatrix+=a_weights.w*invNode*nodes[i]*invBindMatrix[int(a_joints.w)];   
    }
    if(skinMatrix == mat4(0.f)) {
        skinMatrix = mat4(1.f);
    }
    vec4 pos = vec4(a_pos + a_posMorph, 1.0);
    pos = skinMatrix * pos;
    pos = nodes[nodeIndex] * pos;
    vs_out.Pos = (model * pos).xyz;
    gl_Position = worldTransform * pos;

    vec3 normal = a_normal + texture(texture_normal,UV).xyz + a_normMorph;
    vs_out.Normal = normal;
    vec4 tangent = a_tangent + vec4(a_tanMorph,0);
    vs_out.Tangent = tangent.xyz;

    vs_out.Color = a_color_0;
    vs_out.BitTangent = vec3(0,0,0);

}
