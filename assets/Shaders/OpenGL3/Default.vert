#version 330
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 UV1;
layout (location = 2) in vec3 Normal;

layout (location = 6) in ivec4 BoneIDs;
layout (location = 7) in vec4 BoneWeights;

const int MAX_BONES = 64;
uniform mat4 bones[MAX_BONES];
uniform int useBones;
uniform mat4 model;
uniform mat4 viewProjection;
uniform vec3 viewPosition;

out ShaderValues {
	vec3 position;
	vec3 normal;
	vec2 UV;
	vec3 viewDirection;
} outValues;

void main(void) {
	outValues.position = vec3(model * vec4(Position, 1.0));
	outValues.UV = UV1;
	vec3 normal;
	if(useBones == 0) {
		gl_Position = viewProjection * model * vec4(Position, 1.0);
		normal = Normal;
	}
	else {
		mat4 boneTransform  = bones[BoneIDs[0]] * BoneWeights[0];
		if(BoneWeights[1] != 0.0) boneTransform += bones[BoneIDs[1]] * BoneWeights[1];
		if(BoneWeights[2] != 0.0) boneTransform += bones[BoneIDs[2]] * BoneWeights[2];
		if(BoneWeights[3] != 0.0) boneTransform += bones[BoneIDs[3]] * BoneWeights[3];
		normal = vec3(boneTransform * vec4(Normal, 0.0));
		vec4 pos = boneTransform * vec4(Position, 1.0); 
		//pos = pos / pos.w;
		gl_Position = viewProjection * model * pos;
	}
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	
	outValues.normal = normalize(normalMatrix * normal);
	outValues.viewDirection = normalize(viewPosition - outValues.position);
	
}