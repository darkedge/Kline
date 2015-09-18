#pragma once
#if 0
namespace kl {
class DualQuaternion {
public:
	glm::quat m_real;
	glm::quat m_dual;

	/************************************************************************/
	/* Constructors                                                         */
	/************************************************************************/

	DualQuaternion() :
		m_real( 1, 0, 0, 0 ),
		m_dual( 0, 0, 0, 0 ) {}

	DualQuaternion( const glm::quat &real, const glm::quat &dual ) :
		m_real( glm::normalize( real ) ),
		m_dual( dual ) {}

	DualQuaternion( const glm::quat &rotation, const glm::vec3 &translation ) :
		m_real( glm::normalize( rotation ) ), // normalize needed?
		m_dual( ( glm::quat( 0, translation.x, translation.y, translation.z ) * m_real ) * 0.5f ) {}

	DualQuaternion( const glm::quat &rotation ) :
		m_real( glm::normalize( rotation ) ), // normalize needed?
		m_dual( 0, 0, 0, 0 ) {}

	DualQuaternion( const glm::vec3 &translation ) :
		m_real( 1, 0, 0, 0 ),
		m_dual( ( glm::quat( 0, translation.x, translation.y, translation.z ) * m_real ) * 0.5f ) {}

	/************************************************************************/
	/* Other                                                                */
	/************************************************************************/
	// TODO: Change static member functions to free functions

	static float Dot( const DualQuaternion &a, const DualQuaternion &b ) {
		return glm::dot( a.m_real, b.m_real );
	}

	static DualQuaternion Normalize( const DualQuaternion &q ) {
		float mag = glm::dot( q.m_real, q.m_real );
		assert( mag > 0.000001f );
		DualQuaternion ret = q;
		ret.m_real *= 1.0f / mag;
		ret.m_dual *= 1.0f / mag;
		return ret;
	}

	static DualQuaternion Conjugate( const DualQuaternion &q ) {
		return DualQuaternion( glm::conjugate( q.m_real ), glm::conjugate( q.m_dual ) );
	}

	static glm::quat GetRotation( const DualQuaternion &q ) {
		return q.m_real;
	}

	static glm::vec3 QuatXYZ( const glm::quat &q ) {
		return glm::vec3( q.x, q.y, q.z );
	}

	// TODO: Test, optimize
	// Note that for a unit quaternion, the inverse is the same as the conjugate.
	static glm::vec3 TransformPoint( const DualQuaternion &q, const glm::vec3 &v ) {
		//return GetTranslation( q * DualQuaternion( p ) * Conjugate( q ) );
#if 0
		// SIMD code here...
#else
		glm::vec3 r_xyz = QuatXYZ( q.m_real );
		glm::vec3 d_xyz = QuatXYZ( q.m_dual );
		return v +
			   2.0f * glm::cross( r_xyz, ( glm::cross( r_xyz, v ) + q.m_real.w ) ) +
			   2.0f * ( q.m_real.w * d_xyz - q.m_dual.w * r_xyz + glm::cross( r_xyz, d_xyz ) );
#endif
	}

	// TODO: Test, optimize
	static glm::vec3 TransformDirection( const DualQuaternion &q, const glm::vec3 &n ) {
		//return GetRotation( q * DualQuaternion( p ) * Conjugate( q ) );
		glm::vec3 r_xyz = QuatXYZ( q.m_real );
		return n + 2.0f * glm::cross( r_xyz, ( glm::cross( r_xyz, n ) + q.m_real.w * n ) );
	}

	// TODO: Test
	static float Magnitude( const DualQuaternion &q ) {
		return sqrt( Dot( q, Conjugate( q ) ) );
	}

	static glm::vec3 GetTranslation( const DualQuaternion &q ) {
		glm::quat t = 2.0f * q.m_dual * glm::conjugate( q.m_real );
		return glm::vec3( t.x, t.y, t.z );
	}

	static glm::mat4x3 DualQuaternionToMatrix( DualQuaternion q ) {
		q = DualQuaternion::Normalize( q );
		glm::mat4x3 M;

		float w = q.m_real.w;
		float x = q.m_real.x;
		float y = q.m_real.y;
		float z = q.m_real.z;

		// Extract rotational information
		M[0][0] = w * w + x * x	- y * y - z * z;
		M[0][1] = 2 * x * y + 2 * w * z;
		M[0][2] = 2 * x * z - 2 * w * y;
		M[1][0] = 2 * x * y - 2 * w * z;
		M[1][1] = w * w + y * y - x * x - z * z;
		M[1][2] = 2 * y * z + 2 * w * x;
		M[2][0] = 2 * x * z + 2 * w * y;
		M[2][1] = 2 * y * z - 2 * w * x;
		M[2][2] = w * w + z * z - x * x - y * y;

		// Extract translation information
		glm::quat t = ( q.m_dual * 2.0f ) * glm::conjugate( q.m_real );
		M[3][0] = t.x;
		M[3][1] = t.y;
		M[3][2] = t.z;
		return M;
	}
};

/************************************************************************/
/* Operators                                                            */
/************************************************************************/

DualQuaternion operator* ( const DualQuaternion &q, float scale ) {
	DualQuaternion ret = q;
	ret.m_real *= scale;
	ret.m_dual *= scale;
	return ret;
}

DualQuaternion operator+( const DualQuaternion &lhs, const DualQuaternion &rhs ) {
	return DualQuaternion( lhs.m_real + rhs.m_real, lhs.m_dual + rhs.m_dual );
}

// Multiplication order	- left to right
DualQuaternion operator*( const DualQuaternion &lhs, const DualQuaternion &rhs ) {
	return DualQuaternion( rhs.m_real * lhs.m_real,
						   rhs.m_dual * lhs.m_real + rhs.m_real * lhs.m_dual );
}
}
#endif
