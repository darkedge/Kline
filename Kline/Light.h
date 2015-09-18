#pragma once
#include "Component.h"

namespace kl {
class Light : public Component {
public:
	Light( GameObject *gameObject ) : Component( gameObject ) {}

	void SetColor( const glm::vec3 &color );
	glm::vec3 GetColor() const;

private:
	glm::vec3 color = glm::vec3( 1.0f );
};
}