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

out GS_OUT{
  vec3 Pos;
  vec3 Normal;
  vec2 TexCoords;
  vec3 Tangent;
  vec4 Color;
  vec3 BitTangent;
} gs_out;

uniform vec3 viewPos;

const float scale=1.0;

void setOut(int index){
  gs_out.Pos=gl_Position.xyz;
  gs_out.Normal=gs_in[index].Normal;
  gs_out.TexCoords=gs_in[index].TexCoords;
  gs_out.Tangent=gs_in[index].Tangent;
  gs_out.Color=gs_in[index].Color;
  gs_out.BitTangent=gs_in[index].BitTangent;
}

vec4 getAveragePoint(){
  return (gl_in[0].gl_Position+gl_in[1].gl_Position+gl_in[2].gl_Position)/3;
}

vec4 scalePos(vec4 Pos,vec4 average){
  return vec4((Pos.xyz-average.xyz)*scale+average.xyz,Pos.w)-vec4(0,0,1,0)*scale;
}



void main(){
  // vec4 averagePoint=getAveragePoint();

  // gl_Position=scalePos(gl_in[0].gl_Position,averagePoint);
  // setOut(0);
  // gs_out.isOutline=1;
  // EmitVertex();
  // gl_Position=scalePos(gl_in[1].gl_Position,averagePoint);
  // setOut(1);
  // gs_out.isOutline=1;
  // EmitVertex();
  // gl_Position=scalePos(gl_in[2].gl_Position,averagePoint);
  // setOut(2);
  // gs_out.isOutline=1;
  // EmitVertex();
  // EndPrimitive();


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