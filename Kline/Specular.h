#pragma once

#include "Material.h"

namespace kl {
class Texture;

class Specular : public Material {
public:
	void SetDiffuseTexture( Texture *texture );

private:
	// Uniform data
	glm::vec4 diffuseColor;
	glm::vec4 specularColor;
	GLuint diffuseTexture;

	struct {
		GLuint color;
		GLuint normals;
	} textures;

	struct {
		GLint diffuse_albedo;
		GLint specular_albedo;
		GLint shininess;
		GLint diffuse_texture;

		GLint m_matrix;
		GLint mv_matrix;
		GLint view_matrix;
		GLint proj_matrix;
		GLint light_dir; // new
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
