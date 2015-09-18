#include "Common.h"
#include "Specular.h"
#include "Camera.h"
#include "Transform.h"
#include "Texture.h"
#include "LightManager.h"
#include "ShaderLoader.h"
#include "Application.h"

using namespace kl;

// TODO: Might want to keep the reference to the texture and create a bind call there
void Specular::SetDiffuseTexture( Texture *texture ) {
	diffuseTexture = texture->GetHandle();
}

/************************************************************************/
/* Overrides                                                            */
/************************************************************************/
void Specular::Init() {
	ShaderLoader loader;
	handle = loader.Compile(
				 Resource( "shaders/Specular.vert" ),
				 Resource( "shaders/Specular.frag" ) );

	uniforms.diffuse_albedo = glGetUniformLocation( handle, "diffuse_albedo" );
	uniforms.specular_albedo = glGetUniformLocation( handle, "specular_albedo" );
	uniforms.shininess = glGetUniformLocation( handle, "shininess" );
	uniforms.diffuse_texture = glGetUniformLocation( handle, "diffuse_texture" );
}

void Specular::WriteFrameUniformData( Camera *camera, LightManager *lights ) {
	glUniformMatrix4fv( uniforms.view_matrix, 1, GL_FALSE, glm::value_ptr( camera->GetViewMatrix() ) );
	glUniformMatrix4fv( uniforms.proj_matrix, 1, GL_FALSE, glm::value_ptr( camera->GetProjectionMatrix() ) );
	glUniform3fv( uniforms.light_dir, 1, glm::value_ptr( glm::vec3( lights->GetDirectionalLightEuler() ) ) );

	// TODO: Use variables instead of constants
	glUniform1f( uniforms.shininess, 128.0f );
	glUniform3fv( uniforms.diffuse_albedo, 1, glm::value_ptr( glm::vec3( 1.0f ) ) );
	glUniform3fv( uniforms.specular_albedo, 1, glm::value_ptr( glm::vec3( 1.0f ) ) );
}

void Specular::BindInstance() {
	glActiveTexture( GL_TEXTURE0 + 0 );
	glBindTexture( GL_TEXTURE_2D, diffuseTexture );
}

void Specular::WriteObjectUniformData( Camera *camera, Transform *t ) {
	glm::mat4 model_matrix = t->GetLocalToWorldMatrix();
	glUniformMatrix4fv( uniforms.m_matrix, 1, GL_FALSE, glm::value_ptr( model_matrix ) );
	glUniformMatrix4fv( uniforms.mv_matrix, 1, GL_FALSE, glm::value_ptr( camera->GetViewMatrix() * model_matrix ) );
}
