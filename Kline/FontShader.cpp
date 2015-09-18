#include "Common.h"
#include "FontShader.h"
#include "Camera.h"
#include "Transform.h"
#include "Texture.h"
#include "LightManager.h"
#include "ShaderLoader.h"
#include "Application.h"

void kl::FontShader::SetDiffuseTexture( kl::Texture *texture ) {
	diffuseTexture = texture->GetHandle();
}

/************************************************************************/
/* Overrides                                                            */
/************************************************************************/
void kl::FontShader::Init() {
	kl::ShaderLoader loader;
	handle = loader.Compile(
				 kl::Resource( "shaders/FontShader.vert" ),
				 kl::Resource( "shaders/FontShader.frag" ) );

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

void kl::FontShader::WriteFrameUniformData( kl::Camera *, kl::LightManager *lights ) {
	glUniform3fv( uniforms.LightDir, 1, glm::value_ptr( lights->GetDirectionalLightEuler() ) );

	// FIXME: Are these instance specific? Make them assignable
	glUniform3fv( uniforms.diffuse, 1, glm::value_ptr( glm::vec3( 1.0f ) ) );
	glUniform1f( uniforms.AmbientIntensity, 0.0f );
	glUniform1f( uniforms.DiffuseIntensity, 1.0f );
}

void kl::FontShader::BindInstance() {
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, diffuseTexture );
}

void kl::FontShader::WriteObjectUniformData( kl::Camera *camera, kl::Transform *t ) {
	glUniformMatrix4fv( uniforms.m_matrix, 1, GL_FALSE, glm::value_ptr( t->GetLocalToWorldMatrix() ) );
	glUniformMatrix4fv( uniforms.mvp_matrix, 1, GL_FALSE, glm::value_ptr( camera->GetViewProjection() * t->GetLocalToWorldMatrix() ) );
}
