#pragma once

#include "Material.h"

namespace kl {
class Unlit : public Material {
public:
	void SetTexture( GLuint texture );

private:
	GLuint texture;

	struct {
		GLint mvp_matrix;
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