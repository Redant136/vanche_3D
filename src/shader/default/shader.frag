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
    vec3 Color;
    float Intensity;
};
uniform Light lights[MAX_LIGHT_SOURCES];

// color
uniform vec4 baseColorFactor;
uniform bool hasBaseColorTexture;
uniform float alphaCutoff;

// textures
uniform sampler2D texture_base;
uniform sampler2D texture_normal;
uniform sampler2D texture_emisive;
uniform sampler2D texture_occlusion;
uniform sampler2D texture_metalic;
uniform sampler2D texture_roughness;

// extensions
// KHR_materials_unlit
uniform bool KHR_materials_unlit;

// KHR_texture_transform
uniform bool KHR_texture_transform;
uniform vec2 u_offset, u_scale;
uniform float u_rotation;

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
    color += texture(texture_emisive, UV);
  if(!hasBaseColorTexture){
    color = baseColorFactor;
  }else{
    color *= baseColorFactor;
  }
  vec3 normal=fs_in.Normal+texture(texture_normal,UV).xyz;

  for(int i = 0; i < lights.length(); i++)
  {
    if(lights[i].Intensity<=0){
      continue;
    }
    vec3 lightDirection=normalize(lights[i].Position-fs_in.Pos);
    vec4 ambient=lights[i].Intensity*vec4(1,1,1,1);
    vec4 diffuse=max(dot(normalize(normal),lightDirection),0.f)*vec4(lights[i].Color,1);
    color=(ambient+diffuse)*color;
  }
  // color=vec4(1,0,0,1);

  if(color.w<alphaCutoff){
    discard;
  }

  FragColor = color;
}
