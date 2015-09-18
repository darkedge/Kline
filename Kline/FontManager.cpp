#include "Common.h"
#include "FontManager.h"

using namespace kl;

#if 0

FT_Library FontManager::library;
GLuint FontManager::vertexBuffer;

#define MAXWIDTH 1024

struct FontAtlas {
	GLuint texture;
	unsigned int width, height;
	struct Glyph {
		float advanceX;	// advance.x
		float advanceY;	// advance.y

		float bitmapWidth;	// bitmap.width;
		float bitmapHeight;	// bitmap.height;

		float bitmap_left;	// bitmap_left;
		float bitmap_top;	// bitmap_top;

		float texCoordX;	// x offset of glyph in texture coordinates
		float texCoordY;	// y offset of glyph in texture coordinates
	} charInfo[128];		// character information

	FontAtlas( FT_Face face, int height ) {
		FT_Set_Pixel_Sizes( face, 0, height );
		FT_GlyphSlot glyphSlot = face->glyph;

		unsigned int roww = 0;
		int rowh = 0;
		width = 0;
		height = 0;

		memset( charInfo, 0, sizeof charInfo );

		/* Find minimum size for a texture holding all visible ASCII characters */
		for ( int i = 32; i < 128; i++ ) {
			if ( FT_Load_Char( face, i, FT_LOAD_RENDER ) ) {
				fprintf( stderr, "Loading character %c failed!\n", i );
				continue;
			}
			if ( roww + glyphSlot->bitmap.width + 1 >= MAXWIDTH ) {
				width = std::max( width, roww );
				height += rowh;
				roww = 0;
				rowh = 0;
			}
			roww += glyphSlot->bitmap.width + 1;
			rowh = std::max( rowh, glyphSlot->bitmap.rows );
		}

		width = std::max( width, roww );
		height += rowh;

		/* Create a texture that will be used to hold all ASCII glyphs */
		glActiveTexture( GL_TEXTURE0 );
		glGenTextures( 1, &texture );
		glBindTexture( GL_TEXTURE_2D, texture );
		//glUniform1i( uniform_tex, 0 );

		glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0 );

		/* We require 1 byte alignment when uploading texture data */
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

		/* Clamping to edges is important to prevent artifacts when scaling */
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

		/* Linear filtering usually looks best for text */
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		/* Paste all glyph bitmaps into the texture, remembering the offset */
		int ox = 0;
		int oy = 0;

		rowh = 0;

		for ( int i = 32; i < 128; i++ ) {
			if ( FT_Load_Char( face, i, FT_LOAD_RENDER ) ) {
				fprintf( stderr, "Loading character %c failed!\n", i );
				continue;
			}

			if ( ox + glyphSlot->bitmap.width + 1 >= MAXWIDTH ) {
				oy += rowh;
				rowh = 0;
				ox = 0;
			}

			glTexSubImage2D( GL_TEXTURE_2D, 0, ox, oy, glyphSlot->bitmap.width, glyphSlot->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, glyphSlot->bitmap.buffer );
			charInfo[i].advanceX = glyphSlot->advance.x >> 6;
			charInfo[i].advanceY = glyphSlot->advance.y >> 6;

			charInfo[i].bitmapWidth = glyphSlot->bitmap.width;
			charInfo[i].bitmapHeight = glyphSlot->bitmap.rows;

			charInfo[i].bitmap_left = glyphSlot->bitmap_left;
			charInfo[i].bitmap_top = glyphSlot->bitmap_top;

			charInfo[i].texCoordX = ox / ( float )width;
			charInfo[i].texCoordY = oy / ( float )height;

			rowh = std::max( rowh, glyphSlot->bitmap.rows );
			ox += glyphSlot->bitmap.width + 1;
		}

		printf( "Generated a %d x %d (%d kb) texture atlas\n", width, height, width * height / 1024 );
	}

	~FontAtlas() {
		glDeleteTextures( 1, &texture );
	}
};

void FontManager::Init() {
	if ( FT_Init_FreeType( &library ) ) {
		printf( "Could not init freetype library\n" );
		return;
	}
}

void FontManager::CreateFont( const char *path, unsigned int size ) {
	FT_Face face;

	if ( FT_New_Face( library, path, 0, &face ) ) {
		printf( "FT_New_Face failed" );
		return;
	}

	if ( FT_Set_Char_Size( face, size << 6, size << 6, 96, 96 ) != 0 ) {
		printf( "FT_Set_Char_Size failed" );
		return;
	}

}

/**
* Render text using the currently loaded font and currently set font size.
* Rendering starts at coordinates (x, y), z is always 0.
* The pixel coordinates that the FreeType2 library uses are scaled by (sx, sy).
*/
void render_text( const char *text, FontAtlas *a, float x, float y, float sx, float sy ) {
	const uint8_t *p;

	/* Use the texture containing the atlas */
	glBindTexture( GL_TEXTURE_2D, a->texture );
	glUniform1i( uniform_tex, 0 );

	/* Set up the VBO for our vertex data */
	glEnableVertexAttribArray( attribute_coord );
	glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer );
	glVertexAttribPointer( attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0 );

	glm::vec4 coords[6 * strlen( text )];
	int c = 0;

	/* Loop through all characters */
	for ( p = ( const uint8_t * )text; *p; p++ ) {
		FontAtlas::Glyph &bla = a->charInfo[*p];

		/* Calculate the vertex and texture coordinates */
		float x2 = x + bla.bitmap_left * sx;
		float y2 = -y - bla.bitmap_top * sy;
		float w = bla.bitmapWidth * sx;
		float h = bla.bitmapHeight * sy;

		/* Advance the cursor to the start of the next character */
		x += bla.advanceX * sx;
		y += bla.advanceY * sy;

		/* Skip glyphs that have no pixels */
		if ( !w || !h )
			continue;

		coords[c++] = { x2, -y2, bla.texCoordX, bla.texCoordY };
		coords[c++] = { x2 + w, -y2, bla.texCoordX + bla.bitmapWidth / a->width, bla.texCoordY };
		coords[c++] = { x2, -y2 - h, bla.texCoordX, bla.texCoordY + bla.bitmapHeight / a->height };
		coords[c++] = {	x2 + w, -y2, bla.texCoordX + bla.bitmapWidth / a->width, bla.texCoordY };
		coords[c++] = { x2, -y2 - h, bla.texCoordX, bla.texCoordY + bla.bitmapHeight / a->height };
		coords[c++] = {	x2 + w, -y2 - h, bla.texCoordX + bla.bitmapWidth / a->width, bla.texCoordY + bla.bitmapHeight / a->height };
	}

	/* Draw all the character on the screen in one go */
	glBufferData( GL_ARRAY_BUFFER, sizeof( coords ), coords, GL_DYNAMIC_DRAW );
	glDrawArrays( GL_TRIANGLES, 0, c );

	glDisableVertexAttribArray( attribute_coord );
}
#endif
