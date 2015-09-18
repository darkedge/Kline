#include "Common.h"
#include "Texture.h"

// stb libraries by Sean Barrett
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

kl::Texture::Image::Image( const std::string &path ) {
	data = stbi_load( path.c_str(), &w, &h, &n, 0 );
	assert( data );
	if ( n == 3 ) {
		format = GL_RGB;
	} else if ( n == 4 ) {
		format = GL_RGBA;
	}
}

kl::Texture::Image::~Image() {
	stbi_image_free( data );
}

kl::Texture::Texture( unsigned int width, unsigned int height ) {
	glGenTextures( 1, &handle );
	glBindTexture( GL_TEXTURE_2D, handle );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	unsigned int *data = new unsigned int[width * height];
	for ( unsigned int i = 0; i < width * height; i++ ) {
		data[i] = 0xffff00ff;
	}

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );

	delete [] data;

	// Generate mipmaps of active texture unit
	//glGenerateMipmap(GL_TEXTURE_2D);
}

// FIXME: more options for customization
// Wrap mode is clamp
kl::Texture::Texture( const std::string &path, GLenum target ) {
	Image image( path );
	glGenTextures( 1, &handle );
	glBindTexture( target, handle );

	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	if ( target == GL_TEXTURE_1D ) {
		glTexImage1D( target, 0, image.format, image.w, 0, image.format, GL_UNSIGNED_BYTE, image.data );
	} else {
		glTexImage2D( target, 0, image.format, image.w, image.h, 0, image.format, GL_UNSIGNED_BYTE, image.data );
	}

	// Generate mipmaps of active texture unit
	glGenerateMipmap( target );
}

// Regular texture
kl::Texture::Texture( const std::string &path ) {
	Image image( path );
	glGenTextures( 1, &handle );
	glBindTexture( GL_TEXTURE_2D, handle );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	glTexImage2D( GL_TEXTURE_2D, 0, image.format, image.w, image.h, 0, image.format, GL_UNSIGNED_BYTE, image.data );

	// Generate mipmaps of active texture unit
	glGenerateMipmap( GL_TEXTURE_2D );
}

kl::Texture::~Texture() {
	if ( handle )
		glDeleteTextures( 1, &handle );
}

void kl::Texture::LoadCubeMapFace( const std::string &path, GLenum side ) {
	Image image( path );
	glTexImage2D( side, 0, image.format, image.w, image.h, 0, image.format, GL_UNSIGNED_BYTE, image.data );
}

// Cubemap
kl::Texture::Texture( const std::string &posX, const std::string &negX, const std::string &posY, const std::string &negY, const std::string &posZ, const std::string &negZ ) {
	glGenTextures( 1, &handle );
	glBindTexture( GL_TEXTURE_CUBE_MAP, handle );

	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	// This only works because of the glBindTexture call
	LoadCubeMapFace( posX, GL_TEXTURE_CUBE_MAP_POSITIVE_X );
	LoadCubeMapFace( negX, GL_TEXTURE_CUBE_MAP_NEGATIVE_X );
	LoadCubeMapFace( posY, GL_TEXTURE_CUBE_MAP_POSITIVE_Y );
	LoadCubeMapFace( negY, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y );
	LoadCubeMapFace( posZ, GL_TEXTURE_CUBE_MAP_POSITIVE_Z );
	LoadCubeMapFace( negZ, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z );
}

GLuint kl::Texture::GetHandle() {
	return handle;
}
