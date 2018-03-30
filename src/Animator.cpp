#include "Animator.h"
#include "IDGenerator.h"
#include "Engine.h"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "AnimatedModel.h"
#include "Engine.h"
namespace B00289996 {
	

	Animator::Animator() : Component(), playTime(0.0f), dirty(true), currentAnimation(0), animationSpeed(1.0f), boneOffset(glm::vec3(0)){
		
	}

	Animator::~Animator() {
	}
	void Animator::SetModel(const std::shared_ptr<AnimatedModel>& newModel) {
		this->model = newModel;
	}

	const std::shared_ptr<AnimatedModel> Animator::GetModel() const {
		return model;
	}

	void Animator::SetOwner(const std::weak_ptr<Node>& owner) {
		if (owner.use_count() > 0) {
			DestroyConnection("Updater");
		}
		Component::SetOwner(owner);
		if (owner.use_count() > 0) {
			AddConnection(Engine::GetInstance().RegisterWithUpdateCycle(std::bind(&Animator::Update, this, std::placeholders::_1)));
		}
	}
	const std::vector<glm::mat4> &Animator::GetBoneMatrices() {
		boneMatrices.clear();
		if (model) {
			std::vector<aiMatrix4x4> toConvert = model->boneTransforms;
			for (size_t i = 0; i < toConvert.size(); i++) {
				boneMatrices.push_back(AIMat4ToMat4(toConvert[i], boneOffset));
			}
		}
		return boneMatrices;
	}
	/*std::shared_ptr<BoneTransformTree> Animator::GetBoneTransformTree() {
		return model->transformTree;
	}*/
	void Animator::Update(const float & deltaTime) {	
		if (model) {
			playTime += deltaTime * animationSpeed;
			model->Update(playTime);
		}
	}
	void Animator::SetBoneOffset(const glm::vec3 & boneOffset) {
		this->boneOffset = boneOffset;
	}
	void Animator::RegisterWithAnimationSystem(const bool & connect) {
	}

}
