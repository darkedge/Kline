#include "Common.h"
#include "Unlit.h"
#include "ShaderLoader.h"
#include "Camera.h"
#include "Application.h"

using namespace kl;

void Unlit::SetTexture( GLuint texture ) {
	this->texture = texture;
}

/************************************************************************/
/* Overrides                                                            */
/************************************************************************/
void Unlit::Init() {
	ShaderLoader loader;
	handle = loader.Compile(
				 Resource( "shaders/Unlit.vert" ),
				 Resource( "shaders/Unlit.frag" ) );

	// Uniforms
	uniforms.mvp_matrix = glGetUniformLocation( handle, "mvp_matrix" );

	// Samplers
	glUseProgram( handle );
	glUniform1i( glGetUniformLocation( handle, "diffuseSampler" ), 0 );
}

void Unlit::WriteFrameUniformData( Camera *, LightManager * ) {

}

void Unlit::BindInstance() {
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture );
}

void Unlit::WriteObjectUniformData( Camera *camera, Transform *t ) {
	glUniformMatrix4fv( uniforms.mvp_matrix, 1, GL_FALSE, glm::value_ptr( camera->GetViewProjection() * t->GetLocalToWorldMatrix() ) );
}
