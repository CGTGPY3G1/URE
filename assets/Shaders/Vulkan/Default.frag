// Fragment Shader
#version 450
precision highp float;
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
const uint MAX_LIGHTS = 10;
const uint MAX_TEXTURES = 50;
layout (set = 0, binding = 0) uniform sampler2D textures[MAX_TEXTURES];
layout (set = 1, binding = 1) uniform Lights {  
	vec4 position[MAX_LIGHTS];
    vec4 ambient[MAX_LIGHTS];
    vec4 diffuse[MAX_LIGHTS];
    vec4 specular[MAX_LIGHTS];	
	vec4 clqi[MAX_LIGHTS];
	int numberOfLights;
} lights;

layout(push_constant) uniform Material 
{
layout(offset = 144)	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
	int textureIndex;
} material;

layout (location = 0) out vec4 colour;
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 UV;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 viewDirection;

vec3 CalculatePointLighting(int activeLight, float dist) {
    vec3 lightDirection = normalize(lights.position[activeLight].xyz - position);
	float spec = pow(max(dot(normal, normalize(lightDirection + viewDirection)), 0.0), material.shininess);
	float attenuation = 1.0 / (lights.clqi[activeLight].r + lights.clqi[activeLight].g * dist +  lights.clqi[activeLight].b * (dist * dist)); 
    vec4 ambient = lights.ambient[activeLight] * material.ambient;
	float diff = max(dot(normal, lightDirection), 0.0);
    vec4 diffuse = lights.diffuse[activeLight] * diff * material.diffuse;
    vec4 specular = lights.specular[activeLight] * spec * material.specular;
	return ambient.rgb + ((diffuse.rgb + specular.rgb) * attenuation) * lights.clqi[activeLight].a;
}

vec3 CalculateDirectionLighting(int activeLight, vec3 norm) {	
	vec3 lightDirection = normalize(lights.position[activeLight].xyz);
	float diff = max(dot(norm, lightDirection), 0.0);
	float spec = pow(max(dot(viewDirection, reflect(-lightDirection, norm)), 0.0), material.shininess);
	vec3 ambient = lights.ambient[activeLight].xyz * material.ambient.xyz;
	vec3 diffuse = lights.diffuse[activeLight].xyz * diff * material.diffuse.xyz;
	vec3 specular = lights.specular[activeLight].xyz * spec * material.specular.xyz;      
	return ambient + (diffuse + specular) * lights.clqi[activeLight].w;
}

void main() {
	vec4 tex = (material.textureIndex < MAX_TEXTURES) ? texture(textures[material.textureIndex], UV) : vec4(1.0, 1.0, 1.0, 1.0);
	if(lights.numberOfLights > 0) {
		vec3 result = vec3(0.0);
		for(int i = 0; i < lights.numberOfLights; i++) {
			if(lights.position[i].w > 0.5f) {
				float dist = distance(lights.position[i].xyz, position);
				result += CalculatePointLighting(i, dist);
			}
			else {
				result += CalculateDirectionLighting(i, normal);
			}
		}
		colour = vec4(result * tex.rgb, 1.0);
	}
	else colour = material.diffuse * tex;
}