#pragma once
#include "Component.h"
#include "Transform.h"

namespace kl {
class LightManager;
class RenderTexture;
class Camera : public Component {
public:
	Camera( GameObject *gameObject ) : Component( gameObject ) {
		RecalculateProjectionMatrix();
	}

	void Bind();

	enum Projection {
		Perspective,
		Orthographic
	};

	void SetProjection( Projection projection );

	Projection GetProjection() const {
		return projection;
	}

	int GetDepth() const { return depth; }
	float GetZNear() const { return nearPlane; }
	float GetZFar() const {	return farPlane; }
	float GetFieldOfView() const { return fov; }
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetViewProjection();

	RenderTexture *GetRenderTexture() const { return renderTexture; }

	void SetDepth( int depth ) { this->depth = depth; }
	void SetSize( float size );
	void SetNearPlane( float nearPlane );
	void SetFarPlane( float farPlane );
	void SetFieldOfView( float fov );
	void SetViewMatrix( const glm::mat4 &mat ) { this->viewMatrix = mat; }

	void RecalculateProjectionMatrix();

	// Transforms position from screen space into world space.
	glm::vec3 ScreenToWorldPoint( const glm::vec3 &position );
	// Transforms position from world space into screen space.
	glm::vec3 WorldToScreenPoint( const glm::vec3 &position );

private:
	void SetProjectionMatrix( const glm::mat4 &mat ) { this->projectionMatrix = mat; }
	void OnPerspectiveProjectionChanged();
	void OnOrthographicProjectionChanged();

	Projection projection = Perspective;
	float fov = glm::radians( 60.0f ); // For perspective projection
	float size = 5.0f; // For orthographic projection
	float nearPlane = 0.3f;
	float farPlane = 1000.0f;
	int depth = -1;

	RenderTexture *renderTexture = nullptr;

	// Matrices
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
};
} // namespace kl
