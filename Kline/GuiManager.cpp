#include "Common.h"
#include "GuiManager.h"
#include "ShaderLoader.h"
#include "stb_image.h"
#include "MTime.h"
#include "Input.h"
#include "Application.h"

static bool mousePressed[2] = { false, false };
static ImVec2 mousePosScale( 1.0f, 1.0f );

GLuint kl::GuiManager::vertexBuffer;
GLuint kl::GuiManager::handle;
GLuint kl::GuiManager::fontTex;
kl::GuiManager::Uniforms kl::GuiManager::uniforms;
GLuint kl::GuiManager::vao;
float kl::GuiManager::offset = 0.0f;

void kl::GuiManager::Init() {
	int w, h;
	int fb_w, fb_h;
	glfwGetWindowSize( g_Window, &w, &h );
	glfwGetFramebufferSize( g_Window, &fb_w, &fb_h );
	mousePosScale.x = ( float ) fb_w / w;                // Some screens e.g. Retina display have framebuffer size != from window size, and mouse inputs are given in window/screen coordinates.
	mousePosScale.y = ( float ) fb_h / h;

	ImGuiIO &io = ImGui::GetIO();
	io.DisplaySize = ImVec2( ( float ) fb_w, ( float ) fb_h ); // Display size, in pixels. For clamping windows positions.
	io.DeltaTime = 1.0f / 60.0f;                          // Time elapsed since last frame, in seconds (in this sample app we'll override this every frame because our timestep is variable)
	io.PixelCenterOffset = 0.0f;                        // Align OpenGL texels
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;             // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	io.RenderDrawListsFn = ImImpl_RenderDrawLists;
	io.SetClipboardTextFn = ImImpl_SetClipboardTextFn;
	io.GetClipboardTextFn = ImImpl_GetClipboardTextFn;
#if 0
#ifdef _MSC_VER
	io.ImeSetInputScreenPosFn = ImImpl_ImeSetInputScreenPosFn;
#endif
#endif

	// Load font texture
	glGenTextures( 1, &fontTex );
	glBindTexture( GL_TEXTURE_2D, fontTex );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

#if 1
	// Default font (embedded in code)
	const void *png_data;
	unsigned int png_size;
	ImGui::GetDefaultFontData( NULL, NULL, &png_data, &png_size );
	int tex_x, tex_y, tex_comp;
	void *tex_data = stbi_load_from_memory( ( const unsigned char * ) png_data, ( int ) png_size, &tex_x, &tex_y, &tex_comp, 0 );
	IM_ASSERT( tex_data != NULL );
#else
	// Custom font from filesystem
	io.Font = new ImBitmapFont();
	io.Font->LoadFromFile( "../../extra_fonts/mplus-2m-medium_18.fnt" );
	IM_ASSERT( io.Font->IsLoaded() );

	int tex_x, tex_y, tex_comp;
	void *tex_data = stbi_load( "../../extra_fonts/mplus-2m-medium_18.png", &tex_x, &tex_y, &tex_comp, 0 );
	IM_ASSERT( tex_data != NULL );

	// Automatically find white pixel from the texture we just loaded
	// (io.FontTexUvForWhite needs to contains UV coordinates pointing to a white pixel in order to render solid objects)
	for ( int tex_data_off = 0; tex_data_off < tex_x * tex_y; tex_data_off++ )
		if ( ( ( unsigned int * ) tex_data )[tex_data_off] == 0xffffffff ) {
			io.FontTexUvForWhite = ImVec2( ( float )( tex_data_off % tex_x ) / ( tex_x ), ( float )( tex_data_off / tex_x ) / ( tex_y ) );
			break;
		}
#endif

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, tex_x, tex_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data );
	stbi_image_free( tex_data );

	/************************************************************************/
	/* NEW: Material                                                        */
	/************************************************************************/
	kl::ShaderLoader loader;
	handle = loader.Compile(
				 Resource("shaders/GUI.vert"),
				 Resource("shaders/GUI.frag") );

	// Uniforms
	uniforms.mvp_matrix = glGetUniformLocation( handle, "mvp_matrix" );

	// Samplers
	glUseProgram( handle );
	glUniform1i( glGetUniformLocation( handle, "diffuseSampler" ), 0 );

	/************************************************************************/
	/* NEW: Vertex buffer                                                   */
	/************************************************************************/
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	glGenBuffers( 1, &vertexBuffer );
	glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer );

	// Allocate 4 MB vertex attribute buffer without setting data
	glBufferData( GL_ARRAY_BUFFER, 4 * 1024 * 1024, nullptr, GL_STATIC_DRAW );
	// Tell OpenGL where we can find vertex attributes
	// Position
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( ImDrawVert ), nullptr );
	glEnableVertexAttribArray( 0 );
	// Normal
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( ImDrawVert ), ( void * ) 8 );
	glEnableVertexAttribArray( 1 );
	// Texcoord (normalize [0-255] -> [0,1])
	glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof( ImDrawVert ), ( void * ) 16 );
	glEnableVertexAttribArray( 2 );

	// Unbind
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void kl::GuiManager::KeyCallBack( int key, int action, int mods ) {
	ImGuiIO &io = ImGui::GetIO();
	if ( action == GLFW_PRESS )
		io.KeysDown[key] = true;
	if ( action == GLFW_RELEASE )
		io.KeysDown[key] = false;
	io.KeyCtrl = ( mods & GLFW_MOD_CONTROL ) != 0;
	io.KeyShift = ( mods & GLFW_MOD_SHIFT ) != 0;
}

void kl::GuiManager::Update() {
	ImGuiIO &io = ImGui::GetIO();

	// Setup timestep
	static double time = 0.0f;
	const double current_time = glfwGetTime();
	io.DeltaTime = ( float )( current_time - time );
	time = current_time;

	// Setup inputs
	// (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
	if ( !Input::IsMouseGrabbed() ) {
		double mouse_x, mouse_y;
		glfwGetCursorPos( g_Window, &mouse_x, &mouse_y );
		io.MousePos = ImVec2( ( float ) mouse_x * mousePosScale.x, ( float ) mouse_y * mousePosScale.y ); // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)

		io.MouseDown[0] = mousePressed[0] || glfwGetMouseButton( g_Window, GLFW_MOUSE_BUTTON_LEFT ) != 0; // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
		io.MouseDown[1] = mousePressed[1] || glfwGetMouseButton( g_Window, GLFW_MOUSE_BUTTON_RIGHT ) != 0;
	}

	// Start the frame
	ImGui::NewFrame();
}

void kl::GuiManager::Render() {
	ImGui::Render();
}

void kl::GuiManager::Destroy() {
	ImGui::Shutdown();
}

#if 0
#ifdef _MSC_VER
// Notify OS Input Method Editor of text input position (e.g. when using Japanese/Chinese inputs, otherwise this isn't needed)
static void ImImpl_ImeSetInputScreenPosFn( int x, int y ) {
	HWND hwnd = glfwGetWin32Window( g_Window );
	if ( HIMC himc = ImmGetContext( hwnd ) ) {
		COMPOSITIONFORM cf;
		cf.ptCurrentPos.x = x;
		cf.ptCurrentPos.y = y;
		cf.dwStyle = CFS_FORCE_POSITION;
		ImmSetCompositionWindow( himc, &cf );
	}
}
#endif
#endif

// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
// - try adjusting ImGui::GetIO().PixelCenterOffset to 0.5f or 0.375f
void kl::GuiManager::ImImpl_RenderDrawLists( ImDrawList **const cmd_lists, int cmd_lists_count ) {
	if ( cmd_lists_count == 0 )
		return;

	// We are using the OpenGL fixed pipeline to make the example code simpler to read!
	// A probable faster way to render would be to collate all vertices from all cmd_lists into a single vertex buffer.
	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers.
	glDisable( GL_CULL_FACE );
// 	glEnable( GL_BLEND );
// 	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
// 	glDisable( GL_CULL_FACE );
// 	glDisable( GL_DEPTH_TEST );
// 	glEnable( GL_SCISSOR_TEST );
// 	glEnableClientState( GL_VERTEX_ARRAY );
// 	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
// 	glEnableClientState( GL_COLOR_ARRAY );

	// Setup orthographic projection matrix
	const float width = ImGui::GetIO().DisplaySize.x;
	const float height = ImGui::GetIO().DisplaySize.y;

#if 0
	// Setup texture
	glBindTexture( GL_TEXTURE_2D, fontTex );
	glEnable( GL_TEXTURE_2D );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0.0f, width, height, 0.0f, -1.0f, +1.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Render command lists
	for ( int n = 0; n < cmd_lists_count; n++ ) {
		const ImDrawList *cmd_list = cmd_lists[n];
		const unsigned char *vtx_buffer = ( const unsigned char * ) cmd_list->vtx_buffer.begin();
		glVertexPointer( 2, GL_FLOAT, sizeof( ImDrawVert ), ( void * )( vtx_buffer ) );
		glTexCoordPointer( 2, GL_FLOAT, sizeof( ImDrawVert ), ( void * )( vtx_buffer + 8 ) );
		glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( ImDrawVert ), ( void * )( vtx_buffer + 16 ) );

		int vtx_offset = 0;
		const ImDrawCmd *pcmd_end = cmd_list->commands.end();
		for ( const ImDrawCmd *pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++ ) {
			glScissor( ( int ) pcmd->clip_rect.x, ( int )( height - pcmd->clip_rect.w ), ( int )( pcmd->clip_rect.z - pcmd->clip_rect.x ), ( int )( pcmd->clip_rect.w - pcmd->clip_rect.y ) );
			glDrawArrays( GL_TRIANGLES, vtx_offset, pcmd->vtx_count );
			vtx_offset += pcmd->vtx_count;
		}
	}
	glDisable( GL_SCISSOR_TEST );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );
#endif

	/************************************************************************/
	/* Bind material                                                        */
	/************************************************************************/
	glUseProgram( handle );

	// Bind textures
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, fontTex );

	glm::mat4 projection_matrix = glm::ortho( 0.0f, width, height, 0.0f, -1.0f, 1.0f );
	glm::mat4 translation_matrix = glm::translate( glm::vec3( offset, offset, 0 ) );

	// Uniforms
	glUniformMatrix4fv( uniforms.mvp_matrix, 1, GL_FALSE, glm::value_ptr( projection_matrix * translation_matrix ) );

	/************************************************************************/
	/* Render                                                               */
	/************************************************************************/
	glBindVertexArray( vao );
	glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer );
	// Render command lists
	int totalVertexCount = 0;
	for ( int n = 0; n < cmd_lists_count; n++ ) {
		const auto &vertexData = cmd_lists[n]->vtx_buffer;
		int numVertices = (int) vertexData.size();
		// Dump all vertices in VRAM
		glBufferSubData( GL_ARRAY_BUFFER, totalVertexCount * sizeof( ImDrawVert ), numVertices * sizeof( ImDrawVert ), vertexData.begin() );
		totalVertexCount += numVertices;
	}

	glDrawArrays( GL_TRIANGLES, 0, totalVertexCount );

	// Unbind
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glEnable( GL_CULL_FACE );
}

// NB: ImGui already provide OS clipboard support for Windows so this isn't needed if you are using Windows only.
const char *kl::GuiManager::ImImpl_GetClipboardTextFn() {
	return glfwGetClipboardString( g_Window );
}

void kl::GuiManager::ImImpl_SetClipboardTextFn( const char *text ) {
	glfwSetClipboardString( g_Window, text );
}
