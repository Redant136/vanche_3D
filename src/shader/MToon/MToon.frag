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
  flat int isOutline;
} fs_in;



struct Light {
    vec3 Position;
    float Intensity;
    vec3 Color;
    float _pad1;
};
layout (std140) uniform Lights{
  Light lights[MAX_LIGHT_SOURCES];
};

uniform vec4 baseColorFactor;
uniform bool hasBaseColorTexture;
uniform float alphaCutoff;

uniform sampler2D texture_base;
uniform sampler2D texture_emisive;
uniform sampler2D texture_occlusion;
uniform sampler2D texture_metalic;
uniform sampler2D texture_roughness;

// extensions
// KHR_materials_unlit
uniform bool KHR_materials_unlit;

uniform struct VRMData_t{
  vec3 shadeColor;
  sampler2D shadeMultiplyTexture;
  float shadingShiftFactor;
  sampler2D shadingShiftTexture;
  float shadingShiftTextureScale;
  float shadingToonyFactor;
  float outlineWidth;

  float matcapFactor;
  sampler2D matcapTexture;
  vec3 parametricRimColorFactor;
  float parametricRimFresnelPowerFactor;
  float parametricRimLiftFactor;
  sampler2D rimMultiplyTexture;
  float rimLightingMixFactor;

  vec3 outlineColorFactor;
  float outlineLightingMixFactor;
  
} VRMData;


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

  vec4 color = texture(texture_base, UV);
  if(!hasBaseColorTexture){
    color = baseColorFactor;
  }else{
    color *= baseColorFactor;
  }

  if(color.w<alphaCutoff){
    discard;
    return;
  }

  vec3 normal=fs_in.Normal;

  // MATCAP
  vec3 worldViewX=normalize(vec3(fs_in.Pos.z,0,-fs_in.Pos.x));
  vec3 worldViewY=cross(fs_in.Pos,worldViewX);
  vec2 matcapUV=vec2(dot(worldViewX,normal),dot(worldViewY,normal))* 0.495 + 0.5;

  vec3 rim=VRMData.matcapFactor*texture(VRMData.matcapTexture,matcapUV).xyz;
  float parametricRim=clamp(1-dot(normalize(normal),normalize(fs_in.Pos))+VRMData.parametricRimLiftFactor,0,1);
  parametricRim=pow(parametricRim,max(VRMData.parametricRimFresnelPowerFactor,0.00001));
  rim+=parametricRim * VRMData.parametricRimColorFactor; 
  rim*=texture(VRMData.rimMultiplyTexture,UV).xyz;
  rim*=lerp(vec3(1,1,1),color.xyz,VRMData.rimLightingMixFactor);
  color.xyz+=rim;


  // shade toony
  for(int i = 0; i < lights.length(); i++)
  {
    if(lights[i].Intensity<=0){
      continue;
    }
    vec3 lightDir = normalize(lights[i].Position - fs_in.Pos);
    float lightIntensity = dot(fs_in.Normal, lightDir)*lights[i].Intensity;

    lightIntensity += VRMData.shadingShiftFactor;
    lightIntensity += texture(VRMData.shadingShiftTexture, UV).x * VRMData.shadingShiftTextureScale;
    lightIntensity = clamp((lightIntensity-(-1-VRMData.shadingToonyFactor))/((1-VRMData.shadingToonyFactor)-(-1-VRMData.shadingToonyFactor)),0,1);
    
    vec3 shadeColor = VRMData.shadeColor;
    shadeColor *= texture(VRMData.shadeMultiplyTexture,UV).rgb;
    color.xyz = lerp(color.xyz,shadeColor,1-lightIntensity) * lights[i].Color;
  }
  if(fs_in.isOutline==1){
    FragColor=vec4(lerp(VRMData.outlineColorFactor,color.xyz,VRMData.outlineLightingMixFactor),1);
    return;
  }


  FragColor=color;

}