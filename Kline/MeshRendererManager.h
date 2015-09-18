#pragma once
#include "ComponentManager.h"

namespace kl {
class MeshRenderer;
class Camera;
class LightManager;

// TODO: Create a render queue, such that the render order is not defined by the vector.
class MeshRendererManager : public ComponentManager {
public:
	Component *RegisterComponent( Component *component ) override;
	//void UnregisterComponent( Component *component ) override;
	void PostTick() override;

private:
	std::vector<MeshRenderer *> renderables;
};
} // namespace mj
