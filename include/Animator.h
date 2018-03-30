#ifndef UNNAMED_ANIMATOR_COMPONENT_H
#define UNNAMED_ANIMATOR_COMPONENT_H
#include "Component.h"
#include "Transformable.h"
namespace B00289996 {
	constexpr uint32_t MAX_ANIMATION_BONES = 4;
	class AnimatedModel;
	struct BoneTransformTree;

	class Animator : public Component {
	public:
		/// <summary>Initializes a new instance of the <see cref="Animator"/> class. </summary>
		Animator();
		/// <summary>Finalizes an instance of the <see cref="Animator"/> class.</summary>
		~Animator();
		void SetModel(const std::shared_ptr<AnimatedModel> & skeleton);
		const std::shared_ptr<AnimatedModel> GetModel() const;
		virtual const BitMask GetComponentID() const override {
			return TypeInfo::GetComponentID<Animator>();
		}
		virtual const std::shared_ptr<Component> Clone() const override {
			std::shared_ptr<Animator> toReturn = std::make_shared<Animator>(*this);
			toReturn->SetOwner(std::shared_ptr<Node>());
			return std::static_pointer_cast<Component>(toReturn);
		}
		void SetOwner(const std::weak_ptr<Node> & owner) override;
		const std::vector<glm::mat4> & GetBoneMatrices();
		//std::shared_ptr<BoneTransformTree> GetBoneTransformTree();
		void Update(const float & deltaTime);
		void SetBoneOffset(const glm::vec3 & boneOffset);
	private:
		std::uint32_t currentAnimation;
		float playTime = 0.0f, animationSpeed = 0.5f;
		glm::vec3 boneOffset;
		std::vector<glm::mat4> boneMatrices;
		bool dirty;
		void RegisterWithAnimationSystem(const bool & connect);
		bool registeredWithAnimationSystem;
		std::shared_ptr<AnimatedModel> model;
	};
}

#endif // !UNNAMED_ANIMATOR_COMPONENT_H
