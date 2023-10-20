#version 330 core
#define MAX_LIGHT_SOURCES 8

out vec4 FragColor;

in VS_OUT{
  vec3 Pos;
  vec3 Normal;
  vec2 TexCoords;
  vec3 Tangent;
  vec4 Color;
  vec3 BitTangent;
} fs_in;

// lighting
struct Light {
    vec3 Position;
    float Intensity;
    vec3 Color;
    float _pad1;
};
layout (std140) uniform Lights{
  Light lights[MAX_LIGHT_SOURCES];
};

// color
uniform vec4 baseColorFactor;
uniform bool hasBaseColorTexture;
uniform float alphaCutoff;

// textures
uniform sampler2D texture_base;
uniform sampler2D texture_emisive;
uniform sampler2D texture_occlusion;
uniform sampler2D texture_metalic;
uniform sampler2D texture_roughness;

// extensions
// KHR_materials_unlit
uniform bool KHR_materials_unlit;

void main()
{
  vec2 UV=fs_in.TexCoords;

  vec4 color = texture(texture_base, UV);
  if(!hasBaseColorTexture){
    color = baseColorFactor;
  }else{
    color *= baseColorFactor;
  }
  // if(!KHR_materials_unlit)
  //   color += texture(texture_emisive, UV);
  if(color.w<alphaCutoff){
    discard;
    return;
  }

  vec3 normal=fs_in.Normal;

  for(int i = 0; i < lights.length(); i++)
  {
    if(lights[i].Intensity<=0){
      continue;
    }
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lights[i].Color;

    // diffuse
    vec3 lightDirection=normalize(lights[i].Position-fs_in.Pos);
    vec3 diffuse=max(dot(normalize(normal),lightDirection),0.0)*lights[i].Color;

    color.xyz=(ambient+diffuse)*color.xyz;
  }

  FragColor = color;
}
