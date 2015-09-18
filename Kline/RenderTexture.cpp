#include "Common.h"
#include "RenderTexture.h"


kl::RenderTexture::RenderTexture( int width, int height ) :
	width( width ),
	height( height ) {
	glGenFramebuffers( 1, &frameBuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, frameBuffer );
	{
		glGenTextures( 1, &colorTexture );
		glBindTexture( GL_TEXTURE_2D, colorTexture );

		glTexImage2D(
			GL_TEXTURE_2D,		// target
			0,					// level
			GL_RGB,				// internal format
			width,				// width
			height,				// height
			0,					// border
			GL_RGB,				// format
			GL_UNSIGNED_BYTE,	// type
			nullptr );			// pixels

		// Poor filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	}

	{
		glGenTextures( 1, &depthTexture );
		glBindTexture( GL_TEXTURE_2D, depthTexture );

		glTexImage2D(
			GL_TEXTURE_2D,		// target
			0,					// level
			GL_DEPTH_COMPONENT,	// internal format
			width,				// width
			height,				// height
			0,					// border
			GL_DEPTH_COMPONENT,	// format
			GL_UNSIGNED_BYTE,	// type
			nullptr );			// pixels

		// Poor filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	}

	// Attach textures to frame buffer
	glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0 );
	glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture, 0 );

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers( 1, DrawBuffers );

	assert( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE );
}


kl::RenderTexture::~RenderTexture() {
}
