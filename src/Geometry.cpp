#include "Geometry.h"
#include "IDGenerator.h"
namespace B00289996 {

	Mesh::Mesh(const std::string & name) : RenderableResource(), meshName(name), dirty(true), vertices(std::vector<Vertex>()), indices(std::vector<std::uint32_t>()), useBoneData(false), offset(glm::mat4(1.0f)){
		id = IDGenerator<Mesh, std::uint16_t>::GetInstance().GetNewID();
	}

	Mesh::~Mesh() {
		IDGenerator<Mesh, std::uint16_t>::GetInstance().RetireID(id);
	}

	const std::vector<Vertex> Mesh::GetVertices() const {
		return vertices;
	}

	void Mesh::AddVertex(const Vertex & vert) {
		this->vertices.push_back(vert);
		SetDirty();
	}

	void Mesh::AddVertices(const std::vector<Vertex>& verts) {
		this->vertices.insert(this->vertices.end(), verts.begin(), verts.end());
		SetDirty();
	}

	void Mesh::SetVertices(const std::vector<Vertex>& verts) {
		this->vertices = verts;
		SetDirty();
	}

	/*const std::vector<glm::vec3> Mesh::GetPositions() const {
		return positions;
	}

	void Mesh::AddPosition(const glm::vec3 & position) {
		this->positions.push_back(position);
	}

	void Mesh::AddPositions(const std::vector<glm::vec3>& positions) {
		this->positions.insert(positions.end(), positions.begin(), positions.end());
	}

	void Mesh::SetPositions(const std::vector<glm::vec3>& positions) {
		this->positions = positions;
	}

	const std::vector<glm::vec3> Mesh::GetNormals() const {
		return normals;
	}

	void Mesh::AddNormal(const glm::vec3 & norm) {
		normals.push_back(norm);
	}

	void Mesh::AddNormals(const std::vector<glm::vec3>& norms) {
		normals.insert(normals.end(), norms.begin(), norms.end());
	}

	void Mesh::SetNormals(const std::vector<glm::vec3>& norms) {
		normals = norms;
	}

	const std::vector<glm::vec3> Mesh::GetTangents() const {
		return tangents;
	}

	void Mesh::AddTangent(const glm::vec3 & tangent) {
		tangents.push_back(tangent);
	}

	void Mesh::AddTangents(const std::vector<glm::vec3>& tangents) {
		this->tangents.insert(this->tangents.end(), tangents.begin(), tangents.end());
	}

	void Mesh::SetTangents(const std::vector<glm::vec3>& biTangents) {
		this->biTangents = biTangents;
	}

	const std::vector<glm::vec3> Mesh::GetBiTangents() const {
		return biTangents;
	}
	void Mesh::AddBiTangent(const glm::vec3 & bitangent) {
		biTangents.push_back(bitangent);
	}
	void Mesh::AddBiTangents(const std::vector<glm::vec3>& biTangents) {
		this->biTangents.insert(this->biTangents.end(), biTangents.begin(), biTangents.end());
	}

	void Mesh::SetBiTangents(const std::vector<glm::vec3>& biTangents) {
		this->tangents = biTangents;
	}

	const std::vector<glm::vec2> Mesh::GetTextureCoords() const {
		return textureCoords;
	}
	void Mesh::AddTextureCoord(const glm::vec2 & tCoordinate) {
		textureCoords.push_back(tCoordinate);
	}
	void Mesh::AddTextureCoords(const std::vector<glm::vec2>& tCoordinates) {
		this->textureCoords.insert(this->textureCoords.end(), tCoordinates.begin(), tCoordinates.end());
	}

	void Mesh::SetTextureCoords(const std::vector<glm::vec2>& tCoordinates) {
		this->textureCoords = tCoordinates;
	}
	const std::vector<glm::vec4> Mesh::GetColours() const {
		return colours;
	}

	void Mesh::AddColour(const glm::vec4 & colour) {
		colours.push_back(colour);
	}

	void Mesh::AddColours(const std::vector<glm::vec4>& colours) {
		this->colours.insert(this->colours.end(), colours.begin(), colours.end());
	}

	void Mesh::SetColours(const std::vector<glm::vec4>& colours) {
		this->colours = colours;
	}*/

	const size_t Mesh::GetIndexCount() const {
		return indices.size();
	}
	const std::vector<std::uint32_t> Mesh::GetIndices() const {
		return indices;
	}
	void Mesh::AddIndices(const std::vector<std::uint32_t>& meshIndices) {
		this->indices.insert(this->indices.end(), meshIndices.begin(), meshIndices.end());
	}
	void Mesh::SetIndices(const std::vector<std::uint32_t>& meshIndices) {
		indices = meshIndices;
	}
	const AABB Mesh::GetAABB() {
		if (dirty) RecalculateBounds();
		return aabb;
	}

	const Sphere Mesh::GetSphere() {
		if (dirty) RecalculateBounds();
		return Sphere(centre, radius);
	}
	const std::uint16_t Mesh::GetID() const {
		return id;
	}
	void Mesh::SetDirty(const bool & setDirty) {
		if (dirty != setDirty) dirty = setDirty;
	}
	const bool Mesh::UsesBoneData() const {
		return useBoneData;
	}
	void Mesh::SeOffset(const glm::mat4 & offset) {		
		this->offset = offset;
		SetDirty();
	}
	const glm::mat4 Mesh::GetOffset() const {
		return offset;
	}
	void Mesh::RecalculateBounds() {	
		if (vertices.size() > 1) {
			float newRadius = 0.0f;
			glm::vec3 min, max;
			bool first = true;
			for (std::vector<Vertex>::iterator i = vertices.begin(); i < vertices.end(); ++i) {
				const glm::vec3 pos = glm::vec3(offset * glm::vec4((*i).position, 1.0f));
				if (first) {
					first = false;
					min = max = pos;
					newRadius = glm::length(pos);
					continue;
				}			
				// if the distance to the vertex is greater than the current radius
				// set it as the current radius
				const float len = glm::length(pos);
				if (len > newRadius) newRadius = len;
				// if any of the positions components are greater than the minimum/maximum values
				// replace the relevant component with that value
				if (pos.x > max.x) max.x = pos.x;
				else if (pos.x < min.x) min.x = pos.x;
				if (pos.y > max.y) max.y = pos.y;
				else if (pos.y < min.y) min.y = pos.y;
				if (pos.z > max.z) max.z = pos.z;
				else if (pos.z < min.z) min.z = pos.z;
			}
			// update the bounding box and radius
			aabb.min = min; aabb.max = max;
			centre = offset * glm::vec4(((min + max) * 0.5f), 1.0f);
			radius = newRadius;
			SetDirty(false);
		}
	}
}

