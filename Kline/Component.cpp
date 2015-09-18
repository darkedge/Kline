#include "Common.h"
#include "Component.h"
#include "GameObject.h"

bool kl::Component::IsEnabled() const {
	return enabled && this->GetGameObject()->IsEnabled();
}
