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
} gs_out;

void setOut(int index){
  gs_out.Pos=gl_Position.xyz;
  gs_out.Normal=gs_in[index].Normal;
  gs_out.TexCoords=gs_in[index].TexCoords;
  gs_out.Tangent=gs_in[index].Tangent;
  gs_out.Color=gs_in[index].Color;
  gs_out.BitTangent=gs_in[index].BitTangent;
}


void main(){
    gl_Position=gl_in[0].gl_Position;
    setOut(0);
    EmitVertex();
    gl_Position=gl_in[1].gl_Position;
    setOut(1);
    EmitVertex();
    gl_Position=gl_in[2].gl_Position;
    setOut(2);
    EmitVertex();
    EndPrimitive();

}
