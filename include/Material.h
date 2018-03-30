#ifndef UNNAMED_MATERIAL_H
#define UNNAMED_MATERIAL_H
#include "Component.h"
#include "Geometry.h"
#include <memory>
namespace B00289996 {
	class ShaderProgram;
	class Texture;
	class Material {
	public:
		/// <summary>Initializes a new instance of the<see cref="Material"/> class. </summary>
		Material();
		/// <summary>Finalizes an instance of the<see cref="Material"/> class. </summary>
		~Material();
		/// <summary>Replaces the texture specified by the provided index.</summary>
		/// <param name="index">The index.</param>
		/// <param name="newTexture">The new texture.</param>
		/// <param name="addIfNotFound">should this texture be added if the array position indicated by index is empty.</param>
		void ReplaceTexture(const std::size_t & index, const std::shared_ptr<Texture> & newTexture, const bool & addIfNotFound = true);
		/*/// <summary>Replaces the first texture of the specified type?</summary>
		/// <param name="type">The type of the texture.</param>
		/// <param name="newTexture">The new texture.</param>
		/// <param name="addIfNotFound">should this texture be added if no textures of the provided type are found?</param>
		void ReplaceTexture(const TextureType & type, const std::shared_ptr<Texture> & newTexture, const bool & addIfNotFound = true);
		/// <summary>Removes the texture specified by the provided index.</summary>
		/// <param name="index">The array index of the texture.</param>
		void RemoveTexture(const std::size_t & index);
		/// <summary>Removes the texture specified by the provided type.</summary>
		/// <param name="index">The type of the texture.</param>
		void RemoveTextureType(const TextureType & type);*/
		/// <summary>Adds a texture.</summary>
		/// <param name="toAdd">To texture to add.</param>
		void AddTexture(const std::shared_ptr<Texture> & toAdd);
		/// <summary>Adds an array of textures.</summary>
		/// <param name="toAdd">To array of textures to add.</param>
		void AddTextures(const std::vector<std::shared_ptr<Texture>> & toAdd);
		/// <summary>Gets the texture specified by the provided index.</summary>
		/// <param name="index">The array index of the texture.</param>
		/// <returns>the texture specified by the provided index.</returns>
		const std::shared_ptr<Texture> GetTexture(const std::size_t & index) const;
		/// <summary>Gets all textures attached to this material.</summary>
		/// <returns>all textures attached to this material.</returns>
		const std::vector<std::shared_ptr<Texture>> & GetTextures() const;
		void RemoveTextures();
		const std::size_t & GetNumberOfTextures() const;
		/// <summary>Gets the ambient value of this material.</summary>
		/// <returns>the ambient value</returns>
		const glm::vec4 GetAmbient() const;
		/// <summary>Sets the ambient value of this material.</summary>
		/// <param name="newValue">The new ambient value.</param>
		void SetAmbient(const glm::vec4 & newValue);
		/// <summary>Gets the diffuse value of this material.</summary>
		/// <returns>the diffuse value</returns>
		const glm::vec4 GetDiffuse() const;
		/// <summary>Sets the diffuse value of this material.</summary>
		/// <param name="newValue">The new diffuse value.</param>
		void SetDiffuse(const glm::vec4 & newValue);
		/// <summary>Gets the specular value of this material.</summary>
		/// <returns>the specular value</returns>
		const glm::vec4 GetSpecular() const;
		/// <summary>Sets the specular value of this material.</summary>
		/// <param name="newValue">The new specular value.</param>
		void SetSpecular(const glm::vec4 & newValue);
		/// <summary>Gets the shininess of this material.</summary>
		/// <returns>the shininess</returns>
		const float GetShininess() const;
		/// <summary>Sets the shininess of this material.</summary>
		/// <param name="newValue">The new shininess.</param>
		void SetShininess(const float & newValue);
		const std::shared_ptr<ShaderProgram> GetShader() const;
		void SetShader(const std::shared_ptr<ShaderProgram> & shader);
	private:
		glm::vec4 ambient, diffuse, specular;
		float shininess;
		std::vector<std::shared_ptr<Texture>> textures;
		std::shared_ptr<ShaderProgram> shader;
		const static float min, max; //min and max values to clamp the component values between
	};
}

#endif // !UNNAMED_MATERIAL_H
