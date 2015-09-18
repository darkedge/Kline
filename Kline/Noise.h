#pragma once

namespace kl {
class Perlin {
public:
	Perlin();
	Perlin( unsigned int seed );

	float Fade( float t );
	float Noise( float x, float y, float z );
	float Grad( int h, float x, float y, float z );

protected:
	unsigned int seed;
	unsigned char p[512];
};
}