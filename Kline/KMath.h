#pragma once

#if 0
namespace kl {

// Storage: xyz(w)
struct Vector3 {
	Vector3() :
		x( data.m128_f32[0] ),
		y( data.m128_f32[1] ),
		z( data.m128_f32[2] ),
		w( data.m128_f32[3] ) {}

	Vector3( float x, float y, float z ) :
		x( data.m128_f32[0] ),
		y( data.m128_f32[1] ),
		z( data.m128_f32[2] ),
		w( data.m128_f32[3] )  {
		float a[4] = { x, y, z, 0.0f };
		data = _mm_load_ps( a );
	}

	__m128 data;
	float &x, &y, &z, &w;
};

// Storage: xyz(w)
struct Vector4 {
	Vector4() :
		x( data.m128_f32[0] ),
		y( data.m128_f32[1] ),
		z( data.m128_f32[2] ),
		w( data.m128_f32[3] ) {}

	Vector4( float x, float y, float z, float w ) :
		x( data.m128_f32[0] ),
		y( data.m128_f32[1] ),
		z( data.m128_f32[2] ),
		w( data.m128_f32[3] )  {
		float a[4] = { x, y, z, w };
		data = _mm_load_ps( a );
	}

	__m128 data;
	float &x, &y, &z, &w;
};

struct Quaternion {
	Quaternion() :
		x( data.m128_f32[0] ),
		y( data.m128_f32[1] ),
		z( data.m128_f32[2] ),
		w( data.m128_f32[3] ) {}

	Quaternion( float w, float x, float y, float z ) :
		x( data.m128_f32[0] ),
		y( data.m128_f32[1] ),
		z( data.m128_f32[2] ),
		w( data.m128_f32[3] )  {
		float a[4] = { x, y, z, w };
		data = _mm_load_ps( a );
	}

	Vector3 operator*( const Vector3 &rhs ) {
		// SIMD here...
		//return rhs + 2.0 * Cross( Cross( rhs, quat.xyz ) + quat.w * rhs, quat.xyz );
		return rhs;
	}

	__m128 data;
	float &x, &y, &z, &w;
};

struct DualQuaternion {
	Quaternion real;
	Quaternion dual;

	DualQuaternion() {}

	DualQuaternion( float rw, float rx, float ry, float rz, float dw, float dx, float dy, float dz ) :
		real( rw, rx, ry, rz ),
		dual( dw, dx, dy, dz ) {}

	DualQuaternion( const Quaternion &r, const Quaternion &d ) :
		real( r ),
		dual( d ) {}

// 	DualQuaternion( const Quaternion &rotation, const Vector3 &translation ) :
// 		real( Normalize( rotation ) ),
// 		dual( ( Quaternion( 0, translation.x, translation.y, translation.z ) * real ) * 0.5f ) {}
};

Vector3 Cross( const Vector3 &a, const Vector3 &b ) {
	Vector3 result;
	result.data = _mm_sub_ps(
					  _mm_mul_ps( a.data, _mm_shuffle_ps( b.data, b.data, _MM_SHUFFLE( 3, 0, 2, 1 ) ) ),
					  _mm_mul_ps( b.data, _mm_shuffle_ps( a.data, a.data, _MM_SHUFFLE( 3, 0, 2, 1 ) ) )
				  );
	result.data = _mm_shuffle_ps( result.data, result.data, _MM_SHUFFLE( 3, 0, 2, 1 ) );
	return result;
}

Quaternion Normalize( const Quaternion &q ) {
	// TODO
	return q;
}

/************************************************************************/
/* Constants                                                            */
/************************************************************************/
#if 0
static const Vector2 kZeros2f( 0.0f, 0.0f );
static const Vector2 kOnes2f( 1.0f, 1.0f );
static const Vector2 kAxisX2f( 1.0f, 0.0f );
static const Vector2 kAxisY2f( 0.0f, 1.0f );
#endif

static const Vector3 kZeros3f( 0.0f, 0.0f, 0.0f );
static const Vector3 kOnes3f( 1.0f, 1.0f, 1.0f );
static const Vector3 kAxisX3f( 1.0f, 0.0f, 0.0f );
static const Vector3 kAxisY3f( 0.0f, 1.0f, 0.0f );
static const Vector3 kAxisZ3f( 0.0f, 0.0f, 1.0f );

static const Vector4 kZeros4f( 0.0f, 0.0f, 0.0f, 0.0f );
static const Vector4 kOnes4f( 1.0f, 1.0f, 1.0f, 1.0f );
static const Vector4 kAxisX4f( 1.0f, 0.0f, 0.0f, 0.0f );
static const Vector4 kAxisY4f( 0.0f, 1.0f, 0.0f, 0.0f );
static const Vector4 kAxisZ4f( 0.0f, 0.0f, 1.0f, 0.0f );
static const Vector4 kAxisW4f( 0.0f, 0.0f, 0.0f, 1.0f );

static const DualQuaternion kDualQuatIdentity( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
static const Quaternion kQuaternionIdentity( 1.0f, 0.0f, 0.0f, 0.0f );
}
#endif
