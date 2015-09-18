#include "Common.h"
#include "MeshRendererManager.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "Camera.h"
#include "Material.h"

using namespace kl;

Component *MeshRendererManager::RegisterComponent( Component *component ) {
	renderables.push_back( static_cast<MeshRenderer *>( component ) );
	return component;
}

// void MeshRendererManager::UnregisterComponent( Component *component ) {
// 	STL_ERASE_REMOVE( renderables, component );
// 	delete component;
// }

void MeshRendererManager::PostTick() {
	//STL_ERASE_REMOVE_IF( renderables, []( const MeshRenderer *renderable ) { return renderable->GetGameObject()->IsDisposed();} );
	for (auto it = renderables.begin(); it != renderables.end();) {
		MeshRenderer *e = *it;
		if (e->GetGameObject()->IsDisposed()) {
			e->GetMaterial()->RemoveMeshRenderer(e); // Not deleting new'd meshrenderer, leaks memory!
			it = renderables.erase(it);
			continue;
		}
		++it;
	}
}
