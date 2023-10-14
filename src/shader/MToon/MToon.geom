#version 330 core

layout(triangles) in;

layout(triangle_strip,max_vertices=6) out;


in VS_OUT{
  vec3 Pos;
  vec3 Normal;
  vec2 TexCoords;
  vec3 Tangent;
  vec4 Color;
  vec3 BitTangent;
} gs_in[];

out VS_OUT{
  vec3 Pos;
  vec3 Normal;
  vec2 TexCoords;
  vec3 Tangent;
  vec4 Color;
  vec3 BitTangent;
  flat int isOutline;
} gs_out;

uniform sampler2D texture_normal;
uniform bool KHR_texture_transform;
uniform struct KHR_texture_transform_data_t{
  vec2 u_offset, u_scale;
  float u_rotation;
}KHR_texture_transform_data;

uniform mat4 node;// specific node transform
uniform mat4 model;// global model transform
uniform mat4 view;// camera
uniform mat4 projection;// camera

uniform int VRM_outlineWidthMode;
uniform float VRM_outlineWidthFactor;
uniform sampler2D VRM_outlineWidthMultiplyTexture;


void setOut(int index){
  gs_out.Pos=gl_Position.xyz;
  gs_out.Normal=gs_in[index].Normal;
  gs_out.TexCoords=gs_in[index].TexCoords;
  gs_out.Tangent=gs_in[index].Tangent;
  gs_out.Color=gs_in[index].Color;
  gs_out.BitTangent=gs_in[index].BitTangent;
}


void main(){
  for(int i=0;i<3;i++){
    vec2 UV=gs_in[i].TexCoords;
    if(KHR_texture_transform){
      UV = (
        mat3(1,0,0, 0,1,0, KHR_texture_transform_data.u_offset.x, KHR_texture_transform_data.u_offset.y, 1)*
        mat3( cos(KHR_texture_transform_data.u_rotation), sin(KHR_texture_transform_data.u_rotation), 0,
              -sin(KHR_texture_transform_data.u_rotation), cos(KHR_texture_transform_data.u_rotation), 0,
              0,             0, 1)*
        mat3(KHR_texture_transform_data.u_scale.x,0,0, 0,KHR_texture_transform_data.u_scale.y,0, 0,0,1)*
        vec3(gs_in[i].TexCoords,1)).xy;
    }
    vec3 normal=gs_in[i].Normal+texture(texture_normal,UV).xyz;

    vec2 outline=vec2(0,0);
    if(VRM_outlineWidthMode==1){
      outline=normalize(normal).xy*VRM_outlineWidthFactor;
    }else if(VRM_outlineWidthMode==2){
      outline=normalize(normal).xy*VRM_outlineWidthFactor;
    }

    gl_Position = vec4(gl_in[i].gl_Position.xy+outline,gl_in[i].gl_Position.zw)+vec4(0,0,0.01,0);
    setOut(i);
    gs_out.isOutline=1;
    EmitVertex();
  }
  EndPrimitive();


  for(int i=0;i<3;i++){
    gl_Position=gl_in[i].gl_Position;
    setOut(i);
    gs_out.isOutline=0;
    EmitVertex();
  }
  EndPrimitive();

}