#pragma once

#include "Material.h"

namespace kl {
class Texture;
class FontShader : public Material {
public:
	void SetDiffuseTexture( Texture *texture );

private:
	// Uniform data
	glm::vec4 diffuseColor;
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
	void WriteFrameUniformData(Camera *camera, LightManager *lights) override;
	void BindInstance() override;
	void WriteObjectUniformData(Camera *camera, Transform *transform) override;
};
}
