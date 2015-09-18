#pragma once
#include "GameObject.h"

// TODO: System to find other game objects by name
namespace kl {
class Scene {
public:
	virtual void Init() = 0;
	virtual void Destroy() {}
	void PostTick() {
#if 0
		// TODO: This should really be in GameObject's destructor using
		// a shared_ptr or something
		for ( auto it = scene.begin(); it != scene.end(); ) {
			auto g = *it;
			if ( g->IsDisposed() ) {
				g->RemoveComponents();
				it = scene.erase( it );
				continue;
			}
			++it;
		}
#endif
		STL_ERASE_REMOVE_IF( scene, []( const GameObject *go ) { return go->IsDisposed(); } );
	}

	// TODO: Unnamed objects
	GameObject *CreateGameObject( const char *name ) {
		GameObject *go = new GameObject( this, name ) ;
		scene.push_back( go );
		return go;
	}

	virtual ~Scene() {
		for ( GameObject *gameObject : scene ) {
			delete gameObject;
			gameObject = nullptr;
		}
		scene.clear();
	}

	GameObject *Find( const char *name ) {
		auto searchResult = std::find_if( std::begin( scene ), std::end( scene ),
		[&]( GameObject * obj ) -> bool {
			return strcmp( obj->GetName(), name ) == 0;
		} );
		if ( searchResult != std::end( scene ) ) {
			return *searchResult;
		} else {
			return nullptr;
		}
	}

protected:
	std::vector<GameObject *> scene;
};
}
