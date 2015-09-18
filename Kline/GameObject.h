#pragma once
#include "Game.h"
#include "ComponentManager.h"
#include "Transform.h"

namespace kl {
class Transform;
class Component;
class Behavior;
class GameObject {
public:
	GameObject( Scene *scene ) :
		transform( this ) {
		this->scene = scene;
		this->name = "GameObject";
	}
	GameObject( Scene *scene, const char *name ) :
		transform( this ) {
		this->scene = scene;
		this->name = name;
	}
	virtual ~GameObject() {}

	const char *GetName() {
		return name;
	}

	Transform *GetTransform() {
		return &transform;
	}

	// FIXME: Memory leak
	template <class T>
	T *AddComponent() {
		bool isBehavior = std::is_base_of<Behavior, T>::value;

		// Create new component
		T *component = new T( this );
		if ( isBehavior ) {
			Game::GetSubsystem<Behavior>()->RegisterComponent( component );
		} else {
			Game::GetSubsystem<T>()->RegisterComponent( component );
		}

		// FIXME: Game object only contains last set component!
		components[std::type_index( typeid( T ) )] = component;
		return component;
	}

	void Destroy() {
		disposed = true;
		transform.Destroy();
	}

	bool IsDisposed() const {
		return disposed;
	}

	template <class T>
	T *GetComponent() {
		return static_cast<T *>( components.at( std::type_index( typeid( T ) ) ) );
	}

	GameObject *Find( const char *name );
	void SetEnabled( bool enabled ) { this->enabled = enabled; }
	bool IsEnabled() const;

#if 0
	// No way to get the manager of a component...
	void RemoveComponents() {
		for(auto pair : components) {
			Game::GetSubsystem<pair.first>()->UnregisterComponent( pair.second );
		}
	}
#endif

	// FIXME: Access
	Scene *scene;
protected:
	// TODO: Transform is a component
	Transform transform;
	// TODO: Shouldn't this be a multimap?
	std::unordered_map<std::type_index, Component *> components;
	const char *name;
	bool enabled = true;
	bool disposed = false;
};
} // namespace kl
