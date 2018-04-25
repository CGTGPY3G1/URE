#include "DebugDraw.h"
#include "FileInput.h"
#include "Geometry.h"
#include "Camera.h"
#include "ShaderProgram.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>

namespace B00289996 {
	void PShaderError(const GLuint & id, const std::string & prefix = "") {
		GLint logLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
		GLchar * errorLog = new GLchar[logLength];
		int l = 0;
		glGetShaderInfoLog(id, logLength, &l, errorLog);
	}
	// print shader program errors
	void PShaderProgramError(const GLuint & id, const std::string & prefix = "") {
		GLint logLength = 0;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
		GLchar * errorLog = new GLchar[logLength];
		int l = 0;
		glGetProgramInfoLog(id, logLength, &l, errorLog);
	}

	std::string ReadFile(const std::string & location) {
		std::string contents = "";
		std::string line = "";

		std::ifstream file(location, std::ios::in | std::ios::binary);
		if (file.is_open()) {
			while (file.good()) {
				getline(file, line);
				contents.append(line + "\n");
			}
			file.close();
		}
		else std::cout << "File location " + location + " is invalid!" << std::endl;
		return contents;
	}

	GLuint LoadShaderFromString(const std::string & vert, const std::string & frag) {
		// create the required shaders
		GLuint v = glCreateShader(GL_VERTEX_SHADER);
		GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
		// compile the shader sources
		const char * vertSource = vert.c_str();
		GLint vSize = (GLint)vert.size();
		glShaderSource(v, 1, &vertSource, &vSize);
		glCompileShader(v);
		// check the status of the vertex shader
		GLint status;
		glGetShaderiv(v, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			PShaderError(v);
			return 0;
		}

		const char * fragSource = frag.c_str();
		GLint fSize = (GLint)frag.size();
		glShaderSource(f, 1, &fragSource, &fSize);
		// check the status of the fragment shader
		glCompileShader(f);
		glGetShaderiv(f, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			PShaderError(f);
			return 0;
		}
		// create the shader program struct
		GLuint s = 0;
		// generate the shader program and store its handle in the struct 
		s = glCreateProgram();
		// attach the vertex and fragment shaders
		glAttachShader(s, v);
		glAttachShader(s, f);
		// bind the default attribute locations
		glBindAttribLocation(s, VERTEX_POSITION, "position");
		glBindAttribLocation(s, VERTEX_NORMAL, "normal");
		glBindAttribLocation(s, VERTEX_TANGENT, "tangent");
		glBindAttribLocation(s, VERTEX_BITANGENT, "bitangent");
		glBindAttribLocation(s, VERTEX_TEXCOORD, "texCoord");
		glBindAttribLocation(s, VERTEX_COLOUR, "colour");
		// link and bind the shader program
		glGetProgramiv(s, GL_LINK_STATUS, &status);
		if (status != GL_FALSE) std::cout << "Already Linked" << std::endl;
		else {// if not try and link t
			glLinkProgram(s);
			glGetProgramiv(s, GL_LINK_STATUS, &status);
			// if the shader failed to link
			if (status == GL_FALSE) std::cout << "Failed Linking Shader" << std::endl; // report the failure
			glDetachShader(s, v);
			glDetachShader(s, f);
		}
		glUseProgram(s);
		// check to ensure the shader progran id linked correctly
		glGetProgramiv(s, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) {
			PShaderProgramError(s);
			return 0;
		}
		return s;
	}
	GLuint LoadShader(const std::string & vert, const std::string & frag) {
		// try and load the shaders as strings
		const std::string vertData = ReadFile(vert);
		if (vertData.empty()) {
			std::cout << "No vertex shader data found" << std::endl;
			return 0;
		}
		const std::string fragData = ReadFile(frag);
		if (fragData.empty()) {
			std::cout << "No fragment shader data found" << std::endl;
			return 0;
		}

		// load the shaders
		return LoadShaderFromString(vertData, fragData);
	}

	DebugDraw::DebugDraw() : VAO(0), positionBuffer(0), colourBuffer(0), noOfVerts(0), verts(std::vector<glm::vec3>()), colours(std::vector<glm::vec3>()) {
		glLineWidth(2);
		shaderID = LoadShader("../assets/Shaders/OpenGL3\\basic.vert", "../assets/Shaders/OpenGL3/basic.frag");
	}

	void DebugDraw::Init() {
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &positionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verts.size(), verts.data(), GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(VERTEX_POSITION);

		glGenBuffers(1, &colourBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colours.size(), colours.data(), GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)VERTEX_COLOUR, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(VERTEX_COLOUR);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	DebugDraw::~DebugDraw() {
		glDeleteBuffers(1, &colourBuffer);
		glDeleteBuffers(1, &positionBuffer);
		glDeleteVertexArrays(1, &VAO);
		verts.clear();
		colours.clear();
	}

	void DebugDraw::DrawLine(const glm::vec3 & from, const glm::vec3 & to, const glm::vec3 & colour) {
		// add the line to the cache
		verts.push_back(from);
		verts.push_back(to);
		for (size_t i = 0; i < 2; i++) {
			colours.push_back(colour);
		}
		noOfVerts += 2;
	}

	void DebugDraw::DrawSphere(const Sphere & sphere, const glm::vec3 & colour) {
		const int points = 32;
		const float tau = glm::pi<float>() * 2.0f;
		const float increment = tau / points;
		const float radius = sphere.radius;

		std::vector<glm::vec3> x, y, z;
		float angle = 0.0f;
		for (int i = 0; i < points; i++) {
			const float a = glm::cos(angle) * radius, b = glm::sin(angle) * radius;
			glm::vec3 vert = glm::vec3(0.0f, a, b);
			x.push_back(sphere.centre + vert);
			vert.x = a, vert.y = 0.0f; vert.z = b;
			y.push_back(sphere.centre + vert);
			vert.x = a, vert.y = b; vert.z = 0.0f;
			z.push_back(sphere.centre + vert);
			angle += increment;
		}
		const int end = points - 1;
		for (int i = 0; i < end; i++) {
			DrawLine(x[i], x[i + 1]);
			DrawLine(y[i], y[i + 1]);
			DrawLine(z[i], z[i + 1]);
		}
		DrawLine(x[end], x[0]);
		DrawLine(y[end], y[0]);
		DrawLine(z[end], z[0]);
	}

	void DebugDraw::DrawOBB(const OBB & obb, const glm::vec3 & colour) {
		const glm::mat3 rotationMatrix = glm::inverseTranspose(glm::mat3(glm::mat4_cast(obb.rotation)));
		const glm::vec3 right = rotationMatrix * glm::vec3(1.0f, 0.0f, 0.0f), forward = rotationMatrix * glm::vec3(0.0f, 0.0f, 1.0f), up = rotationMatrix * glm::vec3(0.0f, 1.0f, 0.0f);
		float width = obb.extents.x * 2.0f, height = obb.extents.y * 2.0f, length = obb.extents.z * 2.0f;

		const glm::vec3 lbb = rotationMatrix * -obb.extents + obb.centre, rbb = lbb + right * width, rtb = rbb + up * height, ltb = rtb + right * -width,
			lbf = lbb + forward * length, rbf = lbf + right * width, rtf = rbf + up * height, ltf = rtf + right * -width;
		DrawLine(lbb, rbb, colour);
		DrawLine(rbb, rtb, colour);
		DrawLine(rtb, ltb, colour);
		DrawLine(ltb, lbb, colour);
		DrawLine(lbb, lbf, colour);
		DrawLine(rbb, rbf, colour);
		DrawLine(rtb, rtf, colour);
		DrawLine(ltb, ltf, colour);
		DrawLine(lbf, rbf, colour);
		DrawLine(rbf, rtf, colour);
		DrawLine(rtf, ltf, colour);
		DrawLine(ltf, lbf, colour);
	}

	void DebugDraw::Render(const glm::mat4 & viewProjection) {
		if (noOfVerts != 0) {
			if (shaderID) {
				std::shared_ptr<Camera> cam = Camera::GetMainCamera().lock();
				if (cam) {
					glUseProgram(shaderID);
					GLint location = glGetUniformLocation(shaderID, "viewprojection");
					if (location >= 0) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(viewProjection));
					if (VAO == 0) Init();

					glBindVertexArray(VAO);
					glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
					glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verts.size(), verts.data(), GL_STATIC_DRAW);
					glVertexAttribPointer((GLuint)VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
					glEnableVertexAttribArray(VERTEX_POSITION);

					glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
					glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colours.size(), colours.data(), GL_STATIC_DRAW);
					glVertexAttribPointer((GLuint)VERTEX_COLOUR, 3, GL_FLOAT, GL_FALSE, 0, 0);
					glEnableVertexAttribArray(VERTEX_COLOUR);
					glDrawArrays(GL_LINES, 0, (GLsizei)verts.size());
					glBindVertexArray(0);
				}
			}
		}
		FlushLines();
	}
	void DebugDraw::FlushLines() {
		verts.clear();
		colours.clear();
		noOfVerts = 0;
	}
}