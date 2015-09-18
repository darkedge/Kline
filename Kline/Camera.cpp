#include "Common.h"
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "MeshManager.h"
#include "RenderTexture.h"

glm::mat4 kl::Camera::GetViewMatrix() {
	Transform *t = gameObject->GetTransform();
	glm::mat4 translate = glm::translate( -t->GetPosition() );
	glm::mat4 rotate = glm::transpose( glm::toMat4( t->GetRotation() ) );
	return rotate * translate;
}

glm::mat4 kl::Camera::GetProjectionMatrix() {
	return projectionMatrix;
}

void kl::Camera::SetNearPlane( float nearPlane ) {
	this->nearPlane = nearPlane;
	RecalculateProjectionMatrix();
}

void kl::Camera::SetProjection( Camera::Projection projection ) {
	Projection old = this->projection;
	this->projection = projection;
	if ( old != projection ) {
		RecalculateProjectionMatrix();
	}
}

void kl::Camera::SetFarPlane( float farPlane ) {
	this->farPlane = farPlane;
	RecalculateProjectionMatrix();
}

void kl::Camera::SetFieldOfView( float fov ) {
	this->fov = fov;
	OnPerspectiveProjectionChanged();
}

void kl::Camera::SetSize( float size ) {
	this->size = size;
	if ( projection == Camera::Orthographic ) {
		OnOrthographicProjectionChanged();
	}
}

void kl::Camera::RecalculateProjectionMatrix() {
	OnPerspectiveProjectionChanged();
	OnOrthographicProjectionChanged();
}

void kl::Camera::OnPerspectiveProjectionChanged() {
	if ( projection == Camera::Perspective ) {
		SetProjectionMatrix( glm::perspective( fov, ( float ) Game::GetWidth() / ( float ) Game::GetHeight(), nearPlane, farPlane ) );
	}
}

void kl::Camera::OnOrthographicProjectionChanged() {
	if ( projection == Camera::Orthographic ) {
		float right = size * ( float ) Game::GetWidth() / ( float ) Game::GetHeight();
		SetProjectionMatrix( glm::ortho( -right, right, -size, size, nearPlane, farPlane ) );
	}
}

glm::mat4 kl::Camera::GetViewProjection() {
	return GetProjectionMatrix() * GetViewMatrix();
}

// TODO: Customizable clipping planes per camera
void kl::Camera::Bind() {
	// Select framebuffer
	if ( renderTexture ) {
		// Render to texture
		glBindFramebuffer( GL_FRAMEBUFFER, renderTexture->GetFrameBuffer() );
		glViewport( 0, 0, renderTexture->GetWidth(), renderTexture->GetHeight() );
	} else {
		// Render to screen
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		glViewport( 0, 0, Game::GetWidth(), Game::GetHeight() );
	}
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

glm::vec3 kl::Camera::ScreenToWorldPoint( const glm::vec3 &position ) {
	float x = 2.0f * position.x / Game::GetWidth() - 1;
	float y = -2.0f * position.y / Game::GetHeight() + 1;
	glm::mat4 inverseVP = glm::inverse( GetViewProjection() );
	glm::vec4 point3D( x, y, 0, 1 );
	return glm::vec3( inverseVP * point3D ) + position.z * gameObject->GetTransform()->Forward();
}

glm::vec3 kl::Camera::WorldToScreenPoint( const glm::vec3 &position ) {
	glm::mat4 vp = GetViewProjection();
	glm::vec4 point3D = vp * glm::vec4( position, 1.0f );
	float x = ( 1.0f + point3D.x ) / 2.0f * Game::GetWidth();
	float y = ( 1.0f - point3D.y ) / 2.0f * Game::GetHeight();
	return glm::vec3( x, y, 0 );
}
