#version 330

const int MAX_LIGHTS = 10;

uniform struct Light {  
	vec4 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;	
	float constant;
	float linear;
	float quadratic;
	float intensity;
} lights[MAX_LIGHTS];

uniform int numberOfLights;

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};
uniform Material material;
uniform bool useTexture = false;
uniform sampler2D textureUnit0;// diffuse

in ShaderValues {
	vec3 position;
	vec3 normal;
	vec2 UV;
	vec3 viewDirection;
} inValues;

// adapted from https://learnopengl.com/#!Lighting/Light-casters
vec3 CalculatePointLighting(int activeLight, vec3 norm, float dist) {
    vec3 lightDirection = normalize(lights[activeLight].position.xyz - inValues.position);
	float spec = pow(max(dot(norm, normalize(lightDirection + inValues.viewDirection)), 0.0), material.shininess);
	float attenuation = 1.0 / (lights[activeLight].constant + lights[activeLight].linear * dist +  lights[activeLight].quadratic * (dist * dist)); 
    vec3 ambient = lights[activeLight].ambient * material.ambient.xyz;
	float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = lights[activeLight].diffuse * diff * material.diffuse.xyz;
    vec3 specular = lights[activeLight].specular * spec * material.specular.xyz;
	return ambient + ((diffuse + specular) * attenuation) * lights[activeLight].intensity;
}

vec3 CalculateDirectionLighting(int activeLight, vec3 norm) {	
	vec3 lightDirection = normalize(lights[activeLight].position.xyz);
	float diff = max(dot(norm, lightDirection), 0.0);
	float spec = pow(max(dot(inValues.viewDirection, reflect(-lightDirection, norm)), 0.0), material.shininess);
	vec3 ambient = lights[activeLight].ambient * material.ambient.xyz;
	vec3 diffuse = lights[activeLight].diffuse * diff * material.diffuse.xyz;
	vec3 specular = lights[activeLight].specular * spec * material.specular.xyz;      
	return ambient + (diffuse + specular) * lights[activeLight].intensity;
}

out vec4 colour;

void main() {      
	vec4 tex = useTexture ? texture(textureUnit0, inValues.UV) : vec4(1.0, 1.0, 1.0, 1.0);
	if(numberOfLights > 0) {
		vec3 result = vec3(0.0);
		for(int i = 0; i < numberOfLights; i++) {
			if(lights[i].position.w > 0.5f) {
				float dist = distance(lights[i].position.xyz, inValues.position);
				result += CalculatePointLighting(i, inValues.normal, dist);
			}
			else {
				result += CalculateDirectionLighting(i, inValues.normal);
			}
		}
		colour = vec4(result * tex.rgb, 1.0);
	}
	else colour = material.diffuse * tex;
}
