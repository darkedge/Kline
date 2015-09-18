#include "Common.h"
#include "LightManager.h"
#include "Light.h"
#include "GameObject.h"
#include "MeshRendererManager.h"
#include "MeshRenderer.h"
#include "Material.h"

kl::Component *kl::LightManager::RegisterComponent( kl::Component *component ) {
	lights.push_back( static_cast<kl::Light *>( component ) );
	return component;
}

// void LightManager::UnregisterComponent( Component *component ) {
// 	STL_ERASE_REMOVE( lights, component );
// 	delete component;
// }

glm::vec3 kl::LightManager::GetDirectionalLightEuler() {
	assert( !lights.empty() );
	return lights[0]->GetGameObject()->GetTransform()->Forward();
}

void kl::LightManager::PostTick() {
	STL_ERASE_REMOVE_IF( lights, []( const Light *light ) { return light->GetGameObject()->IsDisposed();} );
}
