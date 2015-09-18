#pragma once

#include "Material.h"

namespace kl {
class Texture;
class Diffuse : public Material {
public:
	void SetDiffuseTexture( Texture *texture );
	void SetTexture( GLuint texture );
	void SetDiffuseColor( const glm::vec3 &color );
	GLuint GetTexture();

private:
	glm::vec3 diffuseColor = glm::vec3( 1, 1, 1 );
	GLuint diffuseTexture;

	struct {
		GLint m_matrix;
		GLint mvp_matrix;
		GLint diffuse;
		GLint AmbientIntensity;
		GLint DiffuseIntensity;
		GLint LightDir;
	} uniforms;

	/************************************************************************/
	/* Overrides                                                            */
	/************************************************************************/
public:
	void Init() override;
private:
	void WriteFrameUniformData( Camera *camera, LightManager *lights ) override;
	void BindInstance() override;
	void WriteObjectUniformData( Camera *camera, Transform *transform ) override;
};
}
