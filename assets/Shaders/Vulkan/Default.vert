// Vertex Shader
#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 UV1;
layout (location = 2) in vec3 Normal;


layout (location = 0) out vec3 position;
layout (location = 1) out vec2 UV;
layout (location = 2) out vec3 normal;
layout (location = 3) out vec3 viewDirection;

const int MAX_BONES = 64;
layout(set = 0, binding = 1) uniform Bones {
	mat4 boneArray[MAX_BONES]
} bones;
layout(push_constant) uniform ModelUniforms {
	mat4 model;
	mat4 viewProjection;
	vec3 viewPosition;
} mU;

void main() {
	UV = UV1;
	vec4 pos = mU.model * vec4(Position, 1.0);
	position = vec3(pos);
	mat3 normalMatrix = transpose(inverse(mat3(mU.model)));
	normal = normalize(normalMatrix * Normal);
	viewDirection = normalize(mU.viewPosition - position);
	gl_Position = mU.viewProjection * pos;
}