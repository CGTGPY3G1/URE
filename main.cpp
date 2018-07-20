#include "Engine.h"
#include "Node.h"
#include "TypeInfo.h"
#include "Camera.h"
#include "Geometry.h"
#include "TypeInfo.h"
#include <chrono>
#include "IDGenerator.h"
#include "FileInput.h"
#include "Emitter.h"
#include "MeshRenderer.h"
#include "Light.h"
#include "Debug.h"
#include <glm/gtx/string_cast.hpp>
#include "Animator.h"
#include <iostream>

using namespace B00289996;
float RandomFloat(float a, float b) {
	static bool initialised = false;
	if (!initialised) {
		srand(time(0));
		initialised = true;
	}
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}
void RecursiveRotate(const std::shared_ptr<Transformable> & node, const glm::vec3 & axis, const float angle, const float & scale) {
	node->Rotate(axis, angle * scale);
	std::vector<std::weak_ptr<Transformable>> children = node->GetChildren();
	for (std::vector<std::weak_ptr<Transformable>>::iterator i = children.begin(); i != children.end(); ++i) {
		std::shared_ptr<Transformable> child = (*i).lock();
		if (child) RecursiveRotate(child, axis, angle, -scale * 2.0f);
	}
}
std::shared_ptr<Animator> FindAnimator(const std::shared_ptr<Node> & node) {
	std::shared_ptr<Animator> animator = node->GetComponent<Animator>();
	if (!animator) {
		for (size_t i = 0; i < node->NumberOfChildren(); i++) {
			animator = FindAnimator(std::static_pointer_cast<Node>(node->GetChild(i).lock()));
			if (animator) return animator;
		}
	}
	return animator;
}

void RemoveTextures(std::shared_ptr <Node> &globe) {
	std::shared_ptr<MeshRenderer> globeRenderer = globe->GetChild(0).lock()->GetComponent<MeshRenderer>();
	if (globeRenderer) {
		Material m = globeRenderer->GetMaterial();
		m.RemoveTextures();
		globeRenderer->SetMaterial(m);
	}
}
int main(int argc, char *argv[]) {
	
	Engine & engine = Engine::GetInstance();
	engine.Init(RendererType::OPENGL);
	std::shared_ptr<Node> camera = engine.CreateNode("Camera");
	std::shared_ptr<Camera> cam = camera->AddComponent<Camera>();
	cam->SetAsMain();
//	cam->SetViewPort(0.5f, 0.0f, 1.0f, 1.0f);
	camera->Translate(5.0f, -2.0f, -3.0f);
	camera->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
	/*std::shared_ptr<Node> camera2 = engine.CreateNode("Camera");
	std::shared_ptr<Camera> cam2 = camera2->AddComponent<Camera>();
	cam2->SetViewPort(0.0f, 0.0f, 0.5f, 1.0f);
	camera2->SetPosition(45.0f, 10.0f, 12.5f);
	camera2->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), -90.0f);
	camera2->Rotate(glm::vec3(1.0f, 0.0f, 0.0f), 45.0f);*/
	std::vector<std::shared_ptr<Node>> lights;
	
	

	//lights.push_back(mainLight);
	const float lightIntensity = 5.0f;
	/*std::shared_ptr<Node> light = engine.CreateNode("Light");
	std::shared_ptr<Light> lightComponent = light->AddComponent<Light>();
	lightComponent->SetLightType(LightType::POINT_LIGHT);
	std::shared_ptr<LightStruct> pLight = lightComponent->GetLight();
	pLight->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	pLight->intensity = lightIntensity;
	lights.push_back(light);
	size_t maxX = 10, maxZ = 100;
	bool lightPositioned = false;
	std::vector<std::shared_ptr<Node>> globes;
	for (size_t i = 0; i < maxX; i++) {
		for (size_t j = 0; j < maxZ; j++) {
			std::shared_ptr<Node> globe = engine.LoadModel("Models", "globe.dae");
			globe->SetPosition(i, 0.0f, j);
			globe->Scale(0.5f);
			globe->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), RandomFloat(0.0f, 360.0f));
			globes.push_back(globe);
			if (!lightPositioned && (i == maxX / 2 && j == maxZ / 2)) {
				light->SetPosition(i, 5.0f, j);
				lightPositioned = true;
			}
		}
	}*/
	for (size_t i = 0; i < 5; i++) {
		for (size_t j = 0; j < 5; j++) {
			std::shared_ptr<Node> house = engine.LoadModel("Models", "house2.dae");
			house->SetPosition(+60.0f - i * 10, 0.0f, 30.0f + j * 8);
			house->Rotate(glm::vec3(0.0f, 0.0f, 1.0f), -90.0f);
		}		
	}
	
	/*std::shared_ptr<Node> goblin = engine.LoadAnimatedModel("Models", "goblin.dae");
	std::shared_ptr<Animator> animator = FindAnimator(goblin);
	if (animator) animator->SetBoneOffset(glm::vec3(0.0f, 1750.0f, 0.0f));
	goblin->SetPosition(0.0f, 0.0f, -30.0f);
	goblin->Scale(0.1f);*/


	std::shared_ptr<Node> globe = engine.LoadModel("Models", "globe.dae");
	globe->SetPosition(35.0f, 0.0f, 0.0f);
	globe->Scale(5.0f);
	std::shared_ptr<Node> mainLight = engine.CreateNode("Light");
	std::shared_ptr<Light> mainLightComponent = mainLight->AddComponent<Light>();
	mainLightComponent->SetLightType(LightType::DIRECTION_LIGHT);
	std::shared_ptr<LightStruct> mainPLight = mainLightComponent->GetLight();
	mainLight->SetPosition(100.0f, 100.0f, -100.0f);
	mainPLight->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	mainPLight->intensity = 1.0f;
	lights.push_back(mainLight);
	//size_t maxX = 10, maxY = 10;
	//bool lightPositioned = false;
	//for (size_t i = 0; i < maxX; i++) {
	//	for (size_t j = 0; j < maxY; j++) {
	//		//std::shared_ptr<Node> monkey = engine.LoadModel("Models", "Monkey.dae");
	//		//monkey->SetPosition(i, 0.0f, j);
	//		if (!lightPositioned && (i == maxX / 2 && j == maxY)) {
	//			mainLight->SetPosition(i, 5.0f, j);
	//			lightPositioned = true;
	//		}
	//		std::shared_ptr<Node> globe = engine.LoadModel("Models", "globe.dae");
	//		globe->SetPosition(i, 0.0f, j);
	//	}
	//}

	std::shared_ptr<Node> monkey = engine.LoadModel("Models", "Monkey.dae");
	monkey->SetPosition(25.0f, 0.0f, 0.0f);

	std::shared_ptr<Node> tower = engine.LoadModel("Models", "kltower.obj");
	tower->SetPosition(0.0f, 0.0f, 25.0f);
	tower->Scale(0.1f);
	
	std::shared_ptr<Node> model = engine.LoadModel("Models", "cubes2.dae");

	const float offset = 5.0f;
	
	for (size_t i = 0; i < 8; i++) {
		std::shared_ptr<Node> light = engine.CreateNode("Light");
		light->SetPosition(offset * 2 * (i & 1) - offset, offset * 2 * ((i >> 1) & 1) - offset, offset * 2 * ((i >> 2) & 1) - offset);
		std::shared_ptr<Light> lightComponent = light->AddComponent<Light>();
		lightComponent->SetLightType(LightType::POINT_LIGHT);
		std::shared_ptr<LightStruct> pLight = lightComponent->GetLight();
		if ((i < 4)) {
			if (i == 0) pLight->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			else pLight->diffuse = glm::vec3((i == 1 ? 1.0f : 0.0f), (i == 2 ? 1.0f : 0.0f), (i == 3 ? 1.0f : 0.0f));
		}
		else {
			const size_t wrapped = i % 4;
			if (wrapped == 3) pLight->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			else pLight->diffuse = glm::vec3((wrapped == 2 ? 1.0f : 0.0f), (wrapped == 1 ? 1.0f : 0.0f), (wrapped == 0 ? 1.0f : 0.0f));
		}
		pLight->intensity = lightIntensity;
		lights.push_back(light);
	}

	std::shared_ptr<Node> light = engine.CreateNode("Light");

	std::shared_ptr<Light> lightComponent = light->AddComponent<Light>();
	lightComponent->SetLightType(LightType::POINT_LIGHT);
	std::shared_ptr<LightStruct> pLight = lightComponent->GetLight();
	pLight->diffuse = glm::vec3(1.0f, 0.5f, 0.0f);
	pLight->intensity = 10.0f;
	lights.push_back(light);

	std::uint32_t FPS = 0, framesThisSecond = 0;
	std::chrono::steady_clock::time_point currentTime, lastFrameTime, fpsCheck;
	float tempDelta = 0.0f, deltaTime = 0.0f, totalTime = 0.0f, switchCounter = 0.0f;
	fpsCheck = lastFrameTime = std::chrono::steady_clock::now();
	std::vector<bool> increase(4, true);
	bool paused = false;
	const float MIN_INTENSITY = 0.0f, MAX_INTENSITY = 100.0f;
	const float intensityStep = MAX_INTENSITY * 2.0f;
	while (engine.IsRunning()) {
		currentTime = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration step = (currentTime - lastFrameTime);
		float stepAsFloat = float(step.count()) * std::chrono::steady_clock::period::num / std::chrono::steady_clock::period::den;
		deltaTime = paused ? 0.0f : stepAsFloat;
		totalTime += stepAsFloat;
		switchCounter += stepAsFloat;
		lastFrameTime = currentTime;
		if (float((currentTime - fpsCheck).count()) * std::chrono::steady_clock::period::num / std::chrono::steady_clock::period::den >= 1.0) {
			fpsCheck = currentTime;
			FPS = framesThisSecond;
			framesThisSecond = 0;
			std::cout << "FPS = " << FPS << std::endl;
		}

		glm::vec3 dif = pLight->diffuse;
		for (size_t i = 0; i < 3; i++) {
			if (increase[i]) {
				dif[i] += deltaTime;
				if (dif[i] >= 1.0f) {
					dif[i] = 1.0f;
					increase[i] = false;
				}
			}
			else {
				dif[i] -= deltaTime;
				if (dif[i] <= 0.0f) {
					dif[i] = 0.0f;
					increase[i] = true;
				}
			}
		}
		pLight->diffuse = dif;
		if (increase[3]) {
			pLight->intensity += intensityStep * deltaTime;
			if (pLight->intensity >= MAX_INTENSITY) {
				pLight->intensity = MAX_INTENSITY;
				increase[3] = false;
			}
		}
		else {
			pLight->intensity -= intensityStep * deltaTime;
			if (pLight->intensity <= MIN_INTENSITY) {
				pLight->intensity = MIN_INTENSITY;
				increase[3] = true;
			}
		}
		globe->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 45.0f * deltaTime);
		RecursiveRotate(model, glm::vec3(0.0f, 0.0f, 1.0f), 20.0f * deltaTime, 0.5f);
		engine.Run(deltaTime);
		if (engine.GetKeyDown(KeyboardKey::KEY_SPACE)) {
			engine.SwitchRenderer();
			lastFrameTime = std::chrono::steady_clock::now(); // reset timer
		}
		else {
			const float moveSpeed = 20.0f * stepAsFloat, rotateSpeed = 45.0f * stepAsFloat;
			if (engine.GetKey(KeyboardKey::KEY_W)) camera->Translate(glm::vec3(0.0f, 0.0f, 1.0f) * moveSpeed);
			if (engine.GetKey(KeyboardKey::KEY_S)) camera->Translate(glm::vec3(0.0f, 0.0f, -1.0f) * moveSpeed);
			if (engine.GetKey(KeyboardKey::KEY_A)) camera->Translate(glm::vec3(1.0f, 0.0f, 0.0f) * moveSpeed);
			if (engine.GetKey(KeyboardKey::KEY_D)) camera->Translate(glm::vec3(-1.0f, 0.0f, 0.0f) * moveSpeed);
			if (engine.GetKey(KeyboardKey::KEY_E)) camera->Translate(glm::vec3(0.0f, -1.0f, 0.0f) * moveSpeed);
			if (engine.GetKey(KeyboardKey::KEY_Q)) camera->Translate(glm::vec3(0.0f, 1.0f, 0.0f) * moveSpeed);
			if (engine.GetKey(KeyboardKey::KEY_LEFT)) camera->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), rotateSpeed);
			if (engine.GetKey(KeyboardKey::KEY_RIGHT)) camera->Rotate(glm::vec3(0.0f, -1.0f, 0.0f), rotateSpeed);
			if (engine.GetKey(KeyboardKey::KEY_UP)) camera->Rotate(glm::vec3(-1.0f, 0.0f, 0.0f), rotateSpeed);
			if (engine.GetKey(KeyboardKey::KEY_DOWN)) camera->Rotate(glm::vec3(1.0f, 0.0f, 0.0f), rotateSpeed);
			if (engine.GetKey(KeyboardKey::KEY_RIGHT_CONTROL)) camera->Rotate(glm::vec3(0.0f, 0.0f, 1.0f), rotateSpeed);
			if (engine.GetKey(KeyboardKey::KEY_NUMPAD_0)) camera->Rotate(glm::vec3(0.0f, 0.0f, -1.0f), rotateSpeed);
			if (engine.GetMouseButtonDown(MouseButton::MOUSE_BUTTON_1)) std::cout << glm::to_string(engine.GetMousePosition()) << std::endl;
			if (engine.GetKeyDown(KeyboardKey::KEY_1)) for (auto l : lights) l->ToggleEnabled();
			if (engine.GetKeyDown(KeyboardKey::KEY_ENTER)) paused = !paused;
			if (engine.GetKeyDown(KeyboardKey::KEY_T)) engine.ToggleCulling();
		}
		framesThisSecond++;
	}
	return 1;
}