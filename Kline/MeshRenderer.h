#pragma once
#include "Component.h"

namespace kl {
class GameObject;
class Camera;
class Material;
class LightManager;

class MeshRenderer : public Component {
public:
	MeshRenderer( GameObject *gameObject ) : Component( gameObject ) {}

	void SetMesh( unsigned int mesh );
	unsigned int GetMesh() const { return mesh; }
	void SetMaterial( Material *material );
	Material *GetMaterial() const { return material; }

private:
	Material *material = nullptr;
	unsigned int mesh = 0;
};
} // namespace kl
