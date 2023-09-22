#version 330 core
#define MAX_LIGHT_SOURCES 8

out vec4 FragColor;

in GS_OUT{
  vec3 Pos;
  vec3 Normal;
  vec2 TexCoords;
  vec3 Tangent;
  vec4 Color;
  vec3 BitTangent;
} fs_in;



struct Light {
    vec3 Position;
    vec3 Color;
    float Intensity;
};
uniform Light lights[MAX_LIGHT_SOURCES];
uniform float shadeToony;
uniform float shadeShift;
uniform vec4 materialColor;
uniform vec4 shadeColor;

// color
uniform float outlineWidth;
uniform vec4 outlineColor;
uniform float outlineMix; 

uniform vec4 baseColorFactor;
uniform bool hasBaseColorTexture;
uniform float alphaCutoff;

uniform sampler2D texture_base;
uniform sampler2D texture_normal;
uniform sampler2D texture_emisive;
uniform sampler2D texture_occlusion;
uniform sampler2D texture_metalic;
uniform sampler2D texture_roughness;

uniform vec3 viewPos;

// extensions
// KHR_materials_unlit
uniform bool KHR_materials_unlit;

// KHR_texture_transform
uniform bool KHR_texture_transform;
uniform vec2 u_offset, u_scale;
uniform float u_rotation;


float lerp(float a,float b,float w){
    return a+w*(b-a);
}
vec3 lerp(vec3 a,vec3 b,float w){
    return a+w*(b-a);
}
vec4 lerp(vec4 a,vec4 b,float w){
    return a+w*(b-a);
}
float distanceSquare(vec2 a,vec2 b){
    return (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y);
}
float distanceSquare(vec3 a,vec3 b){
    return (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)+(a.z-b.z)*(a.z-b.z);
}
float distanceSquare(vec4 a,vec4 b){
    return (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)+(a.z-b.z)*(a.z-b.z)+(a.w-b.w)*(a.w-b.w);
}
bool floatEquals(float a,float b){
    return (a-b)<0.001&&(a-b)>-0.001;
}

void main()
{
  vec2 UV=fs_in.TexCoords;
  if(KHR_texture_transform){
    UV = (
      mat3(1,0,0, 0,1,0, u_offset.x, u_offset.y, 1)*
      mat3( cos(u_rotation), sin(u_rotation), 0,
            -sin(u_rotation), cos(u_rotation), 0,
            0,             0, 1)*
      mat3(u_scale.x,0,0, 0,u_scale.y,0, 0,0,1)*
      vec3(fs_in.TexCoords,1.0)).xy;
  }

  vec4 color = texture(texture_base, UV);
  if(!KHR_materials_unlit)
    color += texture(texture_emisive,UV);
  if(!hasBaseColorTexture){
    color = baseColorFactor;
  }else{
    color *= baseColorFactor;
  }

  vec3 normal=fs_in.Normal+texture(texture_normal,UV).xyz;
  
  if(color.w<alphaCutoff){
    discard;
  }
  
  if(outlineWidth>0.01){
    //lerp(col, _OutlineColor * lerp(half3(1, 1, 1), col, _OutlineLightingMix), i.isOutline);
    FragColor=vec4(outlineColor.xyz*lerp(vec3(1),color.xyz,outlineMix),color.w);
    return;
  }


  const float shadeRate=0.2;
  vec3 lighting  = vec3(0,0,0);
  for(int i = 0; i < lights.length(); i++)
  {
    if(lights[i].Intensity<=0){
      continue;
    }
    vec3 lightDir = normalize(lights[i].Position - fs_in.Pos);
    float lightIntensity = max(dot(fs_in.Normal, lightDir), 0.f) * lights[i].Intensity;
    lightIntensity = clamp((lightIntensity - shadeShift) / max(0.000001, (lerp(1,shadeShift,shadeToony) - shadeShift)),0,1);
    // use the clamp to only have a few colors
    lightIntensity = (int(lightIntensity/shadeRate))*shadeRate;
    
    lighting+=lerp(shadeColor,materialColor,lightIntensity).xyz;
    color.xyz=lighting*color.xyz;
  }

  FragColor=color;

}