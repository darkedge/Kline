#pragma once
#include "Component.h"

namespace kl {
class Transform : public Component {
public:
	Transform( GameObject *gameObject ) : Component( gameObject ) {}

	// Transforms position from local space to world space.
	glm::vec3 TransformPoint( const glm::vec3 &point );
	// Transforms direction from local space to world space.
	glm::vec3 TransformDirection( const glm::vec3 &direction ) const;
	// Transforms vector from local space to world space.
	glm::vec3 TransformVector( const glm::vec3 &vector );

	// Returns the matrix that transforms a point from local space into world space.
	glm::mat4 GetLocalToWorldMatrix();
	// Returns the matrix that transforms a point from world space into local space.
	glm::mat4 GetWorldToLocalMatrix();

	// Returns the position of the transform relative to the parent transform.
	glm::vec3 GetLocalPosition() const { return localPosition; }
	// Returns the rotation of the transform relative to the parent transform's rotation.
	glm::quat GetLocalRotation() const { return localRotation; }
	// Returns the scale of the transform relative to the parent.
	glm::vec3 GetLocalScale() const { return localScale; }

	// Returns the position of the transform in world space.
	glm::vec3 GetPosition() const;
	// Returns the rotation of the transform in world space stored as a Quaternion.
	glm::quat GetRotation() const;

	// http://docs.unity3d.com/ScriptReference/Transform-lossyScale.html
	//glm::vec3 GetLossyScale() const;

	// Returns the rotation as Euler angles in radians relative to the parent transform's rotation.
	glm::vec3 GetLocalEulerAngles() const {	return glm::eulerAngles( localRotation ); }

	glm::vec3 Forward() const {
		return localRotation * glm::vec3( 0, 0, -1 );
	}

	glm::vec3 Up() const {
		return localRotation * glm::vec3( 0, 1, 0 );
	}

	glm::vec3 Right() const {
		return localRotation * glm::vec3( 1, 0, 0 );
	}

	// Sets the position of the transform relative to the parent transform.
	void SetLocalPosition( float x, float y, float z );
	void SetLocalPosition( const glm::vec3 &_position );

	// Sets the rotation of the transform relative to the parent transform's rotation.
	void SetLocalRotation( float pitch, float yaw, float roll );
	void SetLocalRotation( const glm::vec3 &degrees );
	void SetLocalRotation( const glm::quat &_orientation );

	// Sets the scale of the transform relative to the parent.
	void SetLocalScale( float xyz );
	void SetLocalScale( float x, float y, float z );
	void SetLocalScale( const glm::vec3 &_scale );

	// Sets the position of the transform in world space.
	void SetPosition( float x, float y, float z );
	void SetPosition( const glm::vec3 &_position );

	// Sets the rotation of the transform in world space.
	void SetRotation( float pitch, float yaw, float roll );
	void SetRotation( const glm::vec3 &degrees );
	void SetRotation( const glm::quat &_orientation );

	// Hierarchy
	Transform *GetParent() const { return parent; }
	void SetParent( Transform *parent, bool worldPositionStays = true );
	void RemoveChild(Transform *transform); // constness?
	void AddChild(Transform *transform); // constness?
	void Destroy();

protected:
	Transform *parent = nullptr;
	std::vector<Transform *> children;

	glm::mat4 localMatrix;
	glm::vec3 localPosition = glm::vec3( 0 );
	glm::quat localRotation = glm::quat( 1, 0, 0, 0 );
	glm::vec3 localScale = glm::vec3( 1 );

	bool matrixIsDirty = true;
};
} // namespace kl
