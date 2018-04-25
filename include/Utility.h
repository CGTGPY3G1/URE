#ifndef UNNAMED_ANIMATED_UTILITY_H
#define UNNAMED_ANIMATED_UTILITY_H
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace B00289996 {

	static const aiMatrix4x4 MakeAIMatrixDecomposable(aiMatrix4x4 toConvert) {
		aiMatrix4x4 conv;
		aiMatrix4x4::RotationX(glm::radians(90.0f), conv);

		return conv * toConvert;
	}

	static const glm::quat AIQuaternionToQuat(const aiQuaternion & toConvert) {
		return glm::quat(toConvert.w, toConvert.x, toConvert.y, toConvert.z);
	}
	static const aiQuaternion QuatToAIQuaternion(const glm::quat & toConvert) {
		return aiQuaternion(toConvert.w, toConvert.x, toConvert.y, toConvert.z);
	}
	static const bool IsChildOf(const aiNode * parent, const aiNode * child) {
		if (child == nullptr || parent == nullptr) return false;
		if (child->mParent == nullptr) return false;
		if (child->mParent == parent) return true;
		return IsChildOf(parent, child->mParent);
	}
	static const aiColor3D Vec4ToAIColor3D(const glm::vec4 & toConvert) {
		return aiColor3D(toConvert.x, toConvert.y, toConvert.z);
	}
	static const glm::vec4 AIColor3DToVec4(const aiColor3D & toConvert, const float w = 1.0f) { return glm::vec4(toConvert.r, toConvert.g, toConvert.b, w); }

	static const aiColor4D Vec4ToAIColor4D(const glm::vec4 & toConvert) {
		return aiColor4D(toConvert.x, toConvert.y, toConvert.z, toConvert.w);
	}

	static const aiVector3D Vec3ToAIVec3(const glm::vec3 & toConvert) {
		return aiVector3D(toConvert.x, toConvert.y, toConvert.z);
	}

	static const aiVector2D Vec2ToAIVec2(const glm::vec2 & toConvert) {
		return aiVector2D(toConvert.x, toConvert.y);
	}

	static const glm::vec4 AIVec3ToVec4(const aiVector3D & toConvert, const float w = 1.0f) {
		return glm::vec4(toConvert.x, toConvert.y, toConvert.z, w);
	}

	static const aiVector3D Vec4ToAIVec3(const glm::vec4 & toConvert) {
		return aiVector3D(toConvert.x, toConvert.y, toConvert.z);
	}

	static const glm::vec3 AIVec3ToVec3(const aiVector3D & toConvert) {
		return glm::vec3(toConvert.x, toConvert.y, toConvert.z);
	}

	static const glm::vec2 AIVec3ToVec2(const aiVector3D & toConvert) {
		return glm::vec2(toConvert.x, toConvert.y);
	}
	static void PrintAIMat4(const aiMatrix4x4 & toPrint) {
		std::cout << toPrint.a1 << "  " << toPrint.b1 << "  " << toPrint.c1 << "  " << toPrint.d1 << std::endl;
		std::cout << toPrint.a2 << "  " << toPrint.b2 << "  " << toPrint.c2 << "  " << toPrint.d2 << std::endl;
		std::cout << toPrint.a3 << "  " << toPrint.b3 << "  " << toPrint.c3 << "  " << toPrint.d3 << std::endl;
		std::cout << toPrint.a4 << "  " << toPrint.b4 << "  " << toPrint.c4 << "  " << toPrint.d4 << std::endl;
	}

	static const glm::mat4 AIMat4ToMat4(const aiMatrix4x4 & toConvert, const glm::vec3 & offset = glm::vec3(0.0f)) {
		glm::mat4x4 toReturn;
		/*assert(sizeof(toReturn) == sizeof(toConvert));
		memcpy(&toReturn, &toConvert, sizeof(toConvert));*/
		//return glm::transpose(toReturn);

		toReturn[0][0] = toConvert.a1; toReturn[0][1] = toConvert.b1;  toReturn[0][2] = toConvert.c1; toReturn[0][3] = toConvert.d1;
		toReturn[1][0] = toConvert.a2; toReturn[1][1] = toConvert.b2;  toReturn[1][2] = toConvert.c2; toReturn[1][3] = toConvert.d2;
		toReturn[2][0] = toConvert.a3; toReturn[2][1] = toConvert.b3;  toReturn[2][2] = toConvert.c3; toReturn[2][3] = toConvert.d3;
		toReturn[3][0] = toConvert.a4 + offset.x; toReturn[3][1] = toConvert.b4 + offset.z;  toReturn[3][2] = toConvert.c4 + offset.y; toReturn[3][3] = toConvert.d4;
		return toReturn;
	}
}
#endif //!UNNAMED_ANIMATED_UTILITY_H
