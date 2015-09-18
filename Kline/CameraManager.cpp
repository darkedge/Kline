#include "Common.h"
#include "CameraManager.h"
#include "Camera.h"
#include "MeshRendererManager.h"
#include "MeshManager.h"
#include "MaterialManager.h"
#include "GameObject.h"

using namespace kl;

Component *CameraManager::RegisterComponent( Component *component ) {
	cameras.push_back( static_cast<Camera *>( component ) );
	return component;
}

// void CameraManager::UnregisterComponent( Component *component ) {
// 	cameras.erase( std::remove( cameras.begin(), cameras.end(), component ), cameras.end() );
// 	delete component;
// }

bool SortCameras( Camera *i, Camera *j ) {
	return ( i->GetDepth() < j->GetDepth() );
}

void CameraManager::Update( LightManager *lights ) {
	// FIXME: Sort on insert? Seems wasteful to sort every frame
	std::sort( std::begin( cameras ), std::end( cameras ), SortCameras );

	// FIXME: This only contains static meshes for now
	// No doubt that we'll need to use multiple buffers sometime
	MeshManager::Bind();

	// For each render target
	for ( Camera *camera : cameras ) {
		// FIXME: Bind call contains skybox rendering
		if ( camera->IsEnabled() ) {
			camera->Bind();
			MaterialManager::Render( camera, lights );
		}
	}
}

void CameraManager::RecalculateProjectionMatrices() {
	for ( Camera *camera : cameras ) {
		camera->RecalculateProjectionMatrix();
	}
}

void CameraManager::PostTick() {
	STL_ERASE_REMOVE_IF( cameras, []( const Camera *camera ) { return camera->GetGameObject()->IsDisposed();} );
}
