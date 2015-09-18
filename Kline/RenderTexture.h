#pragma once

namespace kl {
class RenderTexture {
public:
	RenderTexture( int width, int height );
	~RenderTexture();

	GLuint GetFrameBuffer() const { return frameBuffer; }
	GLuint GetColorTexture() const { return colorTexture; }
	GLuint GetDepthTexture() const { return depthTexture; }
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

private:
	int width = -1;
	int height = -1;
	GLuint colorTexture = 0;
	GLuint depthTexture = 0;
	GLuint frameBuffer = 0;
};
}
