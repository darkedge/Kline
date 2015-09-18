#include "Common.h"
#include "Diffuse.h"
#include "Camera.h"
#include "Transform.h"
#include "Texture.h"
#include "LightManager.h"
#include "ShaderLoader.h"
#include "Application.h"

using namespace kl;

void Diffuse::SetDiffuseTexture( Texture *texture ) {
	diffuseTexture = texture->GetHandle();
}

void Diffuse::SetTexture( GLuint texture ) {
	diffuseTexture = texture;
}

void Diffuse::SetDiffuseColor( const glm::vec3 &color ) {
	this->diffuseColor = color;
}

/************************************************************************/
/* Overrides                                                            */
/************************************************************************/
void Diffuse::Init() {
	ShaderLoader loader;
	handle = loader.Compile(
				 Resource( "shaders/Diffuse.vert" ),
				 Resource( "shaders/Diffuse.frag" ) );

	// Uniforms
	uniforms.m_matrix = glGetUniformLocation( handle, "m_matrix" );
	uniforms.mvp_matrix = glGetUniformLocation( handle, "mvp_matrix" );
	uniforms.diffuse = glGetUniformLocation( handle, "diffuse" );
	uniforms.AmbientIntensity = glGetUniformLocation( handle, "AmbientIntensity" );
	uniforms.DiffuseIntensity = glGetUniformLocation( handle, "DiffuseIntensity" );
	uniforms.LightDir = glGetUniformLocation( handle, "LightDir" );

	// Samplers
	glUseProgram( handle );
	glUniform1i( glGetUniformLocation( handle, "diffuseSampler" ), 0 );
}

void Diffuse::WriteFrameUniformData( Camera *, LightManager *lights ) {
	glUniform1f( uniforms.AmbientIntensity, 0.1f );
	glUniform1f( uniforms.DiffuseIntensity, 1.0f );
	glUniform3fv( uniforms.LightDir, 1, glm::value_ptr( lights->GetDirectionalLightEuler() ) );
}

void Diffuse::BindInstance() {
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, diffuseTexture );
	glUniform3fv( uniforms.diffuse, 1, glm::value_ptr( diffuseColor ) );
}

void Diffuse::WriteObjectUniformData( Camera *camera, Transform *t ) {
	glUniformMatrix4fv( uniforms.m_matrix, 1, GL_FALSE, glm::value_ptr( t->GetLocalToWorldMatrix() ) );
	glUniformMatrix4fv( uniforms.mvp_matrix, 1, GL_FALSE, glm::value_ptr( camera->GetViewProjection() * t->GetLocalToWorldMatrix() ) );
}

GLuint kl::Diffuse::GetTexture() {
	return diffuseTexture;
}
