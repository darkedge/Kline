#include "Common.h"
#include "Material.h"

#include "MeshManager.h"
#include "MeshRenderer.h"
#include "GameObject.h"

using namespace kl;

void Material::Render( Camera *camera, LightManager *lights )  {
	glUseProgram( handle );

	WriteFrameUniformData( camera, lights );

	for ( const auto &instance : instances ) {
		instance->BindInstance();

		// Call all mesh renderers that use this instance
		for ( const auto &meshRenderer : instance->meshRenderers ) {
			if ( meshRenderer->GetGameObject()->IsEnabled() ) {
				// Apply per-object uniforms
				WriteObjectUniformData( camera, meshRenderer->GetGameObject()->GetTransform() );
				MeshManager::DrawMesh( meshRenderer->GetMesh() );
			}
		}
	}
}

void Material::RemoveMeshRenderer( MeshRenderer *meshRenderer ) {
	STL_ERASE_REMOVE( meshRenderers, meshRenderer );
}

void Material::AddMeshRenderer( MeshRenderer *meshRenderer ) {
	if ( STL_CONTAINS( meshRenderers, meshRenderer ) ) return;
	meshRenderers.push_back( meshRenderer );
}

void Material::RemoveInstance( Material *instance )  {
	STL_ERASE_REMOVE( instances, instance );
}
void Material::AddInstance( Material *instance ) {
	if ( !STL_CONTAINS( instances, instance ) ) {
		instances.push_back( instance );
	}
}
