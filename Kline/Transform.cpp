#include "Common.h"
#include "Transform.h"
#include "GameObject.h"

void kl::Transform::SetLocalPosition( float x, float y, float z ) {
	SetLocalPosition( glm::vec3( x, y, z ) );
}

void kl::Transform::SetLocalPosition( const glm::vec3 &_position ) {
	this->localPosition = _position;
	matrixIsDirty = true;
}

void kl::Transform::SetLocalRotation( const glm::quat &_orientation ) {
	this->localRotation = _orientation;
	matrixIsDirty = true;
}

// Degrees
void kl::Transform::SetLocalRotation( float pitch, float yaw, float roll ) {
	SetLocalRotation( glm::vec3( pitch, yaw, roll ) );
}

// Degrees
void kl::Transform::SetLocalRotation( const glm::vec3 &degrees ) {
	SetLocalRotation( glm::quat( glm::radians( degrees ) ) );
}

void kl::Transform::SetLocalScale( float xyz ) {
	SetLocalScale( glm::vec3( xyz ) );
}

void kl::Transform::SetLocalScale( float x, float y, float z ) {
	SetLocalScale( glm::vec3( x, y, z ) );
}

void kl::Transform::SetLocalScale( const glm::vec3 &_scale ) {
	this->localScale = _scale;
	matrixIsDirty = true;
}

glm::mat4 kl::Transform::GetLocalToWorldMatrix() {
	if ( matrixIsDirty ) {
		localMatrix = glm::translate( localPosition )
					  *	glm::toMat4( localRotation )
					  *	glm::scale( localScale );
		matrixIsDirty = false;
	}

	if ( parent ) {
		return parent->GetLocalToWorldMatrix() * localMatrix;
	} else {
		return localMatrix;
	}
}

glm::mat4 kl::Transform::GetWorldToLocalMatrix() {
	return glm::inverse( GetLocalToWorldMatrix() );
}

glm::vec3 kl::Transform::TransformPoint( const glm::vec3 &point ) {
	return glm::vec3( GetLocalToWorldMatrix() * glm::vec4( point, 1 ) );
}

glm::vec3 kl::Transform::TransformDirection( const glm::vec3 &direction ) const {
	return glm::normalize( GetRotation() * direction );
}

glm::vec3 kl::Transform::TransformVector( const glm::vec3 &vector ) {
	return glm::vec3( GetLocalToWorldMatrix() * glm::vec4( vector, 0 ) );
}

glm::vec3 kl::Transform::GetPosition() const {
	if ( parent ) {
		return glm::vec3( parent->GetLocalToWorldMatrix() * glm::vec4( localPosition, 1 ) );
	} else {
		return localPosition;
	}
}

void kl::Transform::SetPosition( const glm::vec3 &_position ) {
	if ( parent ) {
		localPosition = glm::vec3( parent->GetWorldToLocalMatrix() * glm::vec4( _position, 1 ) );
	} else {
		localPosition = _position;
	}
	matrixIsDirty = true;
}

void kl::Transform::SetPosition( float x, float y, float z ) {
	SetPosition( glm::vec3( x, y, z ) );
}

glm::quat kl::Transform::GetRotation() const {
	if ( parent ) {
		return parent->GetRotation() * localRotation;
	} else {
		return localRotation;
	}
}

void kl::Transform::SetRotation( const glm::quat &_orientation ) {
	if ( parent ) {
		localRotation = glm::inverse( parent->GetRotation() ) * _orientation;
	} else {
		localRotation = _orientation;
	}
	matrixIsDirty = true;
}

void kl::Transform::SetRotation( const glm::vec3 &degrees ) {
	SetRotation( glm::quat( glm::radians( degrees ) ) );
}

void kl::Transform::SetRotation( float pitch, float yaw, float roll ) {
	SetRotation( glm::vec3( pitch, yaw, roll ) );
}

// TODO: Scale
void kl::Transform::SetParent( Transform *parent, bool worldPositionStays ) {
	assert( parent != this );
	if ( worldPositionStays ) {
		// Unparent
		if ( parent ) {
			parent->RemoveChild( this );
		}
		glm::vec3 pos = GetPosition();
		glm::quat rot = GetRotation();
		this->parent = parent;
		parent->AddChild(this);
		SetPosition( pos );
		SetRotation( rot );
	}
}

void kl::Transform::RemoveChild( Transform *transform ) {
	STL_ERASE_REMOVE( children, transform );
}

// Too lazy to check for doubles
void kl::Transform::AddChild(Transform *transform) {
	RemoveChild(transform);
	children.push_back(transform);
}

void kl::Transform::Destroy() {
	for(auto &child : children) {
		child->gameObject->Destroy();
	}
}