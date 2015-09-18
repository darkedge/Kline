#include "Common.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "Camera.h"
#include "MeshManager.h"
#include "MaterialManager.h"
#include "Material.h"

using namespace kl;

void MeshRenderer::SetMesh( unsigned int mesh ) {
	this->mesh = mesh;
}

void MeshRenderer::SetMaterial( Material *material ) {
	if(material) {
		material->RemoveMeshRenderer(this);
	}
	this->material = material;
	material->AddMeshRenderer(this);
}
