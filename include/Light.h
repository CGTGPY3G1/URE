#ifndef UNNAMED_LIGHT_H
#define UNNAMED_LIGHT_H
#include "Component.h"
#include <glm/mat4x4.hpp>
#include <memory>
#include <type_traits>
namespace B00289996 {
	enum LightType{
		DIRECTION_LIGHT,
		POINT_LIGHT
	};

	struct LightStruct {
		glm::vec4 position;
		glm::vec3 diffuse = glm::vec3(1.0f);
		glm::vec3 ambient = glm::vec3(0.0f);
		glm::vec3 specular = glm::vec3(0.5f);
		float linear = 1.0f, quadratic = 0.1f, constant = 0.06f, intensity = 1.0f;
	};

	class Light : public Component {
	public:
		/// <summary>Initializes a new instance of the <see cref="Light"/> class. </summary>
		/// <param name="g">The g.</param>
		Light();
		/// <summary>Finalizes an instance of the <see cref="Light"/> class.</summary>
		~Light();
		/// <summary> Gets the type of the light (Point/Directional).</summary>
		/// <returns>the light type</returns>
		const LightType GetLightType() const;
		/// <summary>Sets the type of the light. </summary>
		/// <param name="newType">The new type.</param>
		void SetLightType(const LightType & newType);
		/// <summary> Template for Getting different light types (partial specializations for directional Lights and point lights) </summary>
		/// <returns>the type of light requested (assuming the Light is of that type)</returns>
		std::shared_ptr<LightStruct> GetLight() { 
			return light;
		}

		virtual const BitMask GetComponentID() const override {
			return TypeInfo::GetComponentID<Light>();
		}
		virtual const std::shared_ptr<Component> Clone() const override {
			std::shared_ptr<Light> toReturn = std::make_shared<Light>(*this);
			toReturn->SetOwner(std::shared_ptr<Node>());
			return std::static_pointer_cast<Component>(toReturn);
		}
		void SetOwner(const std::weak_ptr<Node> & owner) override;
	private:
		void RegisterWithLightingSystem(const bool & connect);
		bool registeredWithLightingSystem;
		std::shared_ptr<LightStruct> light;
		LightType lightType;
	};
	
}

#endif // !UNNAMED_LIGHT_H