#pragma once

namespace kl {
class FontShader;
class FontManager {
public:
	static void Init();
	static void CreateFont( const char *path, unsigned int size );
	static void RenderFont( unsigned int font, const char *str );
private:

	static FT_Library library;
	static FontShader *material;
	//static std::vector<Font *> fonts;
	static GLuint vertexBuffer;
};
}
