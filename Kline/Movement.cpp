#include "Common.h"
#include "Movement.h"
#include "GameObject.h"
#include "Transform.h"
#include "Input.h"
#include "MTime.h"

using namespace kl;

Transform *transform;

void Movement::Start() {
	transform = gameObject->GetTransform();
}

void Movement::Update() {
	if ( Input::GetKeyDown( GLFW_KEY_M ) ) {
		Input::SetMouseGrabbed( !Input::IsMouseGrabbed() );
	}

	if ( Input::IsMouseGrabbed() ) {
		// Rotation
		const float ROT_SPEED = 0.0025f;
		currentMousePos -= ROT_SPEED * Input::GetMouseDelta();
		if ( currentMousePos.y < glm::radians( -89.0f ) ) {
			currentMousePos.y = glm::radians( -89.0f );
		}
		if ( currentMousePos.y > glm::radians( 89.0f ) ) {
			currentMousePos.y = glm::radians( 89.0f );
		}
		if ( currentMousePos.x != lastMousePos.x || currentMousePos.y != lastMousePos.y ) {
			transform->SetRotation( glm::quat( glm::vec3( currentMousePos.y, currentMousePos.x, 0 ) ) );
			lastMousePos = currentMousePos;
		}
	}

	// Translation
	const float SPEED = 1.0f;
	glm::vec3 translation;
	glm::quat orientation = transform->GetRotation();
	if ( Input::GetKey( GLFW_KEY_W ) )		translation += transform->Forward();
	if ( Input::GetKey( GLFW_KEY_A ) )		translation -= transform->Right();
	if ( Input::GetKey( GLFW_KEY_S ) )		translation -= transform->Forward();
	if ( Input::GetKey( GLFW_KEY_D ) )		translation += transform->Right();
	if ( Input::GetKey( GLFW_KEY_LEFT_CONTROL ) || Input::GetKey( GLFW_KEY_C ) || Input::GetKey( GLFW_KEY_LEFT_SHIFT ) ) translation -= glm::vec3( 0, 1, 0 );
	if ( Input::GetKey( GLFW_KEY_SPACE ) ) translation += glm::vec3( 0, 1, 0 );
	if ( translation != glm::vec3() ) {
		glm::vec3 pos = transform->GetPosition();
		pos += glm::normalize( translation ) * SPEED * Time::GetDeltaTime();

		transform->SetPosition( pos );
	}

	// Reset
	if ( Input::GetKey( GLFW_KEY_R ) ) {
		currentMousePos = lastMousePos = { 0, 0 };
		transform->SetPosition( glm::vec3() );
		transform->SetRotation( glm::quat( 1, 0, 0, 0 ) );
	}

	//printf( "%.1f %.1f %.1f\n", transform->GetPosition().x, transform->GetPosition().y, transform->GetPosition().z );
}
