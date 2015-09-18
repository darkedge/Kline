#pragma once
#include "MaterialManager.h"

namespace kl {
class GameObject;
class MeshRenderer;
class Camera;
class LightManager;
class Transform;

class Material {
public:

	// Only called for the first instance
	virtual void Init() = 0;

	void Render( Camera *camera, LightManager *lights );
	void RemoveMeshRenderer( MeshRenderer *meshRenderer );

	// Adds a mesh renderer to this material.
	// Adding it more than once has no effect.
	void AddMeshRenderer( MeshRenderer *meshRenderer );

	template <class T>
	static T *Create() {
		return MaterialManager::Create<T>();
	}

	void AddInstance( Material *instance );

	void RemoveInstance( Material *instance );

protected:
	// Called by derived classes that load for the first time
	Material() {
		instances.push_back( this );
	}

	virtual void WriteFrameUniformData( Camera *camera, LightManager *lights) = 0;
	virtual void BindInstance() = 0;
	virtual void WriteObjectUniformData( Camera *camera, Transform *transform ) = 0;

	GLuint handle;
private:
	std::vector<Material *> instances;

	// List of mesh renderers that this material instance is bound to
	std::vector<MeshRenderer *> meshRenderers;
};
}
