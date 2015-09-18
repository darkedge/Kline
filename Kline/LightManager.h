#pragma once
#include "ComponentManager.h"

namespace kl {
class Light;
class MeshRendererManager;

// TODO: Only supports 1 directional light
class LightManager : public ComponentManager {
public:
	Component *RegisterComponent( Component *component ) override;
	//void UnregisterComponent( Component *component ) override;
	void PostTick() override;

	glm::vec3 GetDirectionalLightEuler();

private:
	std::vector<Light *> lights;
};
}
