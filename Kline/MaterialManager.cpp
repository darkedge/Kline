#include "Common.h"
#include "MaterialManager.h"

#include "MeshRenderer.h"
#include "MeshManager.h"

#include "Material.h"

using namespace kl;

std::unordered_map<std::type_index, Material *> MaterialManager::materials;

void MaterialManager::Render( Camera *camera, LightManager *lights ) {
	for ( const auto &mat : materials ) {
		mat.second->Render( camera, lights );
	}
}
