#pragma once

namespace kl {
class Component;
class ComponentManager {
public:
	virtual Component *RegisterComponent( Component *component ) = 0;
	//virtual void UnregisterComponent( Component *component ) = 0;
	virtual void PostTick() = 0;
protected:

};
}
