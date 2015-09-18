#pragma once
#include "Behavior.h"

namespace kl {
class Transform;
class Mesh;
class Movement : public Behavior {
public:
	Movement( GameObject *gameObject ) : Behavior( gameObject ) {}

	void Start() override;
	void Update() override;

private:
	glm::vec2 lastMousePos = {0, 0};
	glm::vec2 currentMousePos = {0, 0};
};
} // namespace mj
