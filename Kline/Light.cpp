#include "Common.h"
#include "Light.h"

using namespace kl;

void Light::SetColor( const glm::vec3 &color ) {
	this->color = color;
}

glm::vec3 Light::GetColor() const {
	return color;
}
