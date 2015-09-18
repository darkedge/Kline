#pragma once

namespace kl {
class Texture {
public:
	Texture( const std::string &path, GLenum target );
	Texture( unsigned int width, unsigned int height );
	// Deprecated, use Texture(path, GL_TEXTURE_2D)
	Texture( const std::string &path );
	Texture( const std::string &posX,
			 const std::string &negX,
			 const std::string &posY,
			 const std::string &negY,
			 const std::string &posZ,
			 const std::string &negZ );

	~Texture();

	GLuint GetHandle();

private:
	Texture();

	GLuint handle;

	struct Image {
		Image( const std::string &path );
		~Image();
		int w, h, n;
		unsigned char *data;
		GLenum format;
	};

	void LoadCubeMapFace( const std::string &path, GLenum side );
};
}
