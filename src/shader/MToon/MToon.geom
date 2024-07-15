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

uniform int VRM_outlineWidthMode;
uniform float VRM_outlineWidthFactor;
uniform sampler2D VRM_outlineWidthMultiplyTexture;


void setOut(int index){
  gs_out.Pos=gs_in[index].Pos;
  gs_out.Normal=gs_in[index].Normal;
  gs_out.TexCoords=gs_in[index].TexCoords;
  gs_out.Tangent=gs_in[index].Tangent;
  gs_out.Color=gs_in[index].Color;
  gs_out.BitTangent=gs_in[index].BitTangent;
}


void main(){
  // loop through points in reverse to inverse the direction the triangle is drawn in so that this is the backface of the vertice
  for(int i=2;i>=0;i--){
    vec3 normal=gs_in[i].Normal;

    vec2 outline=vec2(0,0);
    if(VRM_outlineWidthMode==1){
      // world coordinates
      outline=normalize(normal).xy*VRM_outlineWidthFactor;
    }else if(VRM_outlineWidthMode==2){
      // screen coordinates
      // TODO(ANT) test
      outline=normalize(normal).xy*VRM_outlineWidthFactor*gl_in[i].gl_Position.z;
    }

    gl_Position = vec4(gl_in[i].gl_Position.xy+outline,gl_in[i].gl_Position.zw);
    setOut(i);
    gs_out.isOutline=1;
    EmitVertex();
  }
  EndPrimitive();

  // draw actual vertice
  for(int i=0;i<3;i++){
    gl_Position=gl_in[i].gl_Position;
    setOut(i);
    gs_out.isOutline=0;
    EmitVertex();
  }
  EndPrimitive();

}