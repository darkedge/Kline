#pragma once
#include "ComponentManager.h"

namespace kl {
class Behavior;

class BehaviorManager : public ComponentManager {
public:
	BehaviorManager() {}
	virtual ~BehaviorManager() {}

	Component *RegisterComponent( Component *component ) override;
	//void UnregisterComponent( Component *component ) override;
	void PostTick() override;

	void Start() const;
	void Update() const;

protected:
	std::vector<Behavior *> behaviors;
};
} // namespace kl
