#pragma once
#include "Component.h"

namespace kl {
class GameObject;

class Behavior : public Component {
public:
	Behavior( GameObject *gameObject ) : Component( gameObject ) {}

	virtual void Start() {}
	virtual void Update() {}
};
} // namespace mj
