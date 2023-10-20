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


layout (std140,packed) uniform Transforms{
    mat4 worldTransform;// camera and global model transform
    mat4 model; // global model transform
    int nodeIndex;// specific node transform index
    int _pad1[3]; // for padding issues
    int jointNodes[MAX_JOINT_MATRIX]; // array of joint nodes index
};

layout (std140) uniform Nodes{
    mat4 nodes[MAX_NODES]; // all node transforms in the model
};

layout (std140) uniform InverseBindMatrices{
    mat4 invBindMatrix[MAX_JOINT_MATRIX]; // inverse bind matrices
}; 

uniform int texCoordIndex;
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
    vs_out.Normal = a_normal;

    mat4 skinMatrix = mat4(0.f);
    mat4 invNode = inverse(nodes[nodeIndex]);
    if(a_joints.x<jointNodes.length()&&a_joints.x>-1){
        skinMatrix+=a_weights.x*invNode*nodes[jointNodes[int(a_joints.x)]]*invBindMatrix[int(a_joints.x)];
    }
    if(a_joints.y<jointNodes.length()&&a_joints.y>-1){
        skinMatrix+=a_weights.y*invNode*nodes[jointNodes[int(a_joints.y)]]*invBindMatrix[int(a_joints.y)];
    }
    if(a_joints.z<jointNodes.length()&&a_joints.z>-1){
        skinMatrix+=a_weights.z*invNode*nodes[jointNodes[int(a_joints.z)]]*invBindMatrix[int(a_joints.z)];
    }
    if(a_joints.w<jointNodes.length()&&a_joints.w>-1){
        skinMatrix+=a_weights.w*invNode*nodes[jointNodes[int(a_joints.w)]]*invBindMatrix[int(a_joints.w)];   
    }
    if(skinMatrix == mat4(0.f))
    {
        skinMatrix = mat4(1.f);
    }
    vec4 pos = vec4(a_pos,1.0);
    pos = pos + vec4(a_posMorph,0);
    pos = skinMatrix * pos;
    pos = nodes[nodeIndex] * pos;
    vs_out.Pos = (model * pos).xyz;
    pos = worldTransform * pos;

    vec3 normal = (worldTransform * vec4(a_normal,1)).xyz;
    vec4 tangent = a_tangent;


    vs_out.Normal = normal;
    vs_out.Tangent = tangent.xyz;
    vs_out.Color = a_color_0;
    vs_out.BitTangent = vec3(0,0,0);

    gl_Position = pos;
}
