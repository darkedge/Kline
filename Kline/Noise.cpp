#include "Common.h"
#include "Noise.h"

using namespace kl;

Perlin::Perlin() : Perlin( std::random_device()() ) {}

Perlin::Perlin( unsigned int seed ) {
	this->seed = seed;
	std::mt19937 g( seed );

	unsigned char a[256];
	std::iota( std::begin( a ), std::end( a ), 0 );
	std::shuffle( std::begin( a ), std::end( a ), g );
	std::memcpy( p, a, 256 );
	std::memcpy( p + 256, a, 256 );
}

float Perlin::Fade( float t ) {
	return t * t * t * ( t * ( t * 6 - 15 ) + 10 );
}

float Perlin::Grad( int hash, float x, float y, float z ) {
	int h = hash & 15;
	// Convert lower 4 bits of hash into 12 gradient directions
	float u = h < 8 ? x : y,
		  v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ( ( h & 1 ) == 0 ? u : -u ) + ( ( h & 2 ) == 0 ? v : -v );
}

float Perlin::Noise( float x, float y, float z ) {
	// Find the unit cube that contains the point
	int X = ( int )( std::floor( x ) ) & 0xff;
	int Y = ( int )( std::floor( y ) ) & 0xff;
	int Z = ( int )( std::floor( z ) ) & 0xff;

	// Find relative x, y,z of point in cube
	x -= std::floor( x );
	y -= std::floor( y );
	z -= std::floor( z );

	// Compute fade curves for each of x, y, z
	float u = Fade( x );
	float v = Fade( y );
	float w = Fade( z );

	// Hash coordinates of the 8 cube corners
	int A = p[X + 0] + Y;
	int AA = p[A + 0] + Z;
	int AB = p[A + 1] + Z;
	int B = p[X + 1] + Y;
	int BA = p[B + 0] + Z;
	int BB = p[B + 1] + Z;

	// Add blended results from 8 corners of cube
	float res =
		glm::mix(
			glm::mix(
				glm::mix(
					Grad( p[AA + 0], x - 0, y - 0, z - 0 ),
					Grad( p[BA + 0], x - 1, y - 0, z - 0 ),
					u
				),
				glm::mix(
					Grad( p[AB + 0], x - 0, y - 1, z - 0 ),
					Grad( p[BB + 0], x - 1, y - 1, z - 0 ),
					u
				),
				v
			),
			glm::mix(
				glm::mix(
					Grad( p[AA + 1], x - 0, y - 0, z - 1 ),
					Grad( p[BA + 1], x - 1, y - 0, z - 1 ),
					u
				),
				glm::mix(
					Grad( p[AB + 1], x - 0, y - 1, z - 1 ),
					Grad( p[BB + 1], x - 1, y - 1, z - 1 ),
					u
				),
				v
			),
			w
		);
	return res;
}