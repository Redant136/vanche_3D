#version 330 core
#define MAX_JOINT_MATRIX 128

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec4 a_tangent;
layout (location = 3) in vec2 a_texCoords_0;
layout (location = 4) in vec2 a_texCoords_1;
layout (location = 5) in vec2 a_texCoords_2;
layout (location = 6) in vec4 a_color_0;
layout (location = 7) in vec4 a_joints;
layout (location = 8) in vec4 a_weights;


uniform int texCoordIndex;

out VS_OUT{
    vec3 Pos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec4 Color;
    vec3 BitTangent;
} vs_out;

uniform mat4 u_jointMatrix[MAX_JOINT_MATRIX];// joint matrices

uniform mat4 node;// specific node transform
uniform mat4 model;// global model transform
uniform mat4 view;// camera
uniform mat4 projection;// camera

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

    mat4 skinMatrix = mat4(1.f);
    if(a_joints.x<u_jointMatrix.length()&&a_joints.y<u_jointMatrix.length()&&a_joints.z<u_jointMatrix.length()&&a_joints.w<u_jointMatrix.length()
        &&a_joints.x>-1&&a_joints.y>-1&&a_joints.z>-1&&a_joints.w>-1)
    {
        skinMatrix=
            a_weights.x*u_jointMatrix[int(a_joints.x)]+
            a_weights.y*u_jointMatrix[int(a_joints.y)]+
            a_weights.z*u_jointMatrix[int(a_joints.z)]+
            a_weights.w*u_jointMatrix[int(a_joints.w)];
        if(skinMatrix == mat4(0.f))
        {
            skinMatrix = mat4(1.f);
        }
    }
    vec4 pos = vec4(a_pos,1.0);
    pos = skinMatrix * pos;
    pos = projection * view * model * node * pos;

    vec3 normal = a_normal;
    vec4 tangent = a_tangent;


    vs_out.Pos = pos.xyz;
    vs_out.Normal = normal;
    vs_out.Tangent = tangent.xyz;
    vs_out.Color = a_color_0;
    vs_out.BitTangent = vec3(0,0,0);

    gl_Position = pos;
}
