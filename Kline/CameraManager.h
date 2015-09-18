#pragma once
#include "ComponentManager.h"

namespace kl {
class Camera;
class LightManager;

class CameraManager : public ComponentManager {
public:
	Component *RegisterComponent( Component *component ) override;
	//void UnregisterComponent( Component *component ) override;
	void PostTick() override;

	void Update( LightManager *lights );

	void RecalculateProjectionMatrices();
protected:
	std::vector<Camera *> cameras;
};
}