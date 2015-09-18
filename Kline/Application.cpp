#include "Common.h"
#include "Application.h"

#include "Game.h"
#include "Input.h"
#include "MTime.h"
#include "GuiManager.h"

GLFWwindow *g_Window;

LARGE_INTEGER now, lastTime, ticksPS;

std::string kl::Application::resourceFolder;

std::string kl::Resource( const char *file ) {
	return Application::GetResourceFolder() + "\\" + file;
}

/************************************************************************/
/* GLFW Callbacks                                                       */
/************************************************************************/
static void GlfwErrorCallback( int, const char *description ) {
	fputs( description, stderr );
	fputs( "\n", stderr );
}
static void GlfwMouseButtonCallback( GLFWwindow *, int button, int action, int ) {
	if ( action == GLFW_PRESS ) {
		kl::Input::SetMouseButton( button, true );
	}
	if ( action == GLFW_RELEASE ) {
		kl::Input::SetMouseButton( button, false );
	}
}

static void GlfwScrollCallback( GLFWwindow *, double, double yoffset ) {
	ImGuiIO &io = ImGui::GetIO();
	io.MouseWheel = ( yoffset != 0.0f ) ? yoffset > 0.0f ? 1 : -1 : 0;         // Mouse wheel: -1,0,+1
}

static void GlfwKeyCallBack( GLFWwindow *window, int key, int, int action, int mods ) {
	if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose( window, GL_TRUE );

	if ( action == GLFW_PRESS ) {
		kl::Input::SetKey( key, true );
	}
	if ( action == GLFW_RELEASE ) {
		kl::Input::SetKey( key, false );
	}

	kl::GuiManager::KeyCallBack( key, action, mods );
}

static void GlfwCursorPosCallback( GLFWwindow *, double x, double y ) {
	kl::Input::SetMousePosition( glm::vec2( x, y ) );
}

static void GlfwCharCallback( GLFWwindow *, unsigned int c ) {
	if ( c > 0 && c < 0x10000 )
		ImGui::GetIO().AddInputCharacter( ( unsigned short ) c );
}

static void GlfwWindowSizeCallBack( GLFWwindow *, int width, int height ) {
	kl::Game::SetResolution( width, height );

	// Resize ImGUI
	int fb_w, fb_h;
	glfwGetFramebufferSize( g_Window, &fb_w, &fb_h );

	// FIXME: Quick hack to prevent Imgui crashing
	if ( width > 0 && height > 0 )
		ImGui::GetIO().DisplaySize = ImVec2( ( float ) fb_w, ( float ) fb_h ); // Display size, in pixels. For clamping windows positions.
}

/************************************************************************/
/* Initialization                                                       */
/************************************************************************/
// http://www.glfw.org/docs/latest/window.html#window_hints_values
static void InitGLFW() {
	glfwSetErrorCallback( GlfwErrorCallback );

	if ( !glfwInit() )
		exit( 1 );

	// Window hints
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE );
#ifdef _DEBUG
	glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE );
#endif

	g_Window = glfwCreateWindow( kl::Game::GetWidth(), kl::Game::GetHeight(), "KlineEngine by Marco Jonkers", nullptr, nullptr );
	glfwMakeContextCurrent( g_Window );

	// Callbacks
	glfwSetKeyCallback( g_Window, GlfwKeyCallBack );
	glfwSetMouseButtonCallback( g_Window, GlfwMouseButtonCallback );
	glfwSetScrollCallback( g_Window, GlfwScrollCallback );
	glfwSetCharCallback( g_Window, GlfwCharCallback );
	glfwSetCursorPosCallback( g_Window, GlfwCursorPosCallback );
	glfwSetWindowSizeCallback( g_Window, GlfwWindowSizeCallBack );

	// Vsync
	glfwSwapInterval( 1 );
}

static void InitGL() {
	if ( !gladLoadGL() ) {
		printf( "Failed to init glad!\n" );
		exit( -1 );
	}
	glEnable( GL_BLEND );
	glEnable( GL_MULTISAMPLE );
	glEnable( GL_CULL_FACE );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
	glClearColor( 49 / 255.0f, 77 / 255.0f, 121 / 255.0f, 1.0f );// Unity's default clear color
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDepthFunc( GL_LEQUAL );
}

void kl::Application::Start( kl::Scene *scene ) {
	InitGLFW();
	InitGL();

	{
		// Find resource folder
		const std::string resFolderName = "res"; // Make constant?
		kl::Path p = Path::CurrentPath();
		bool foundResourceFolder = false;
		for ( int i = 0; i < 5; i++ ) {
			p = p.Parent();
			if ( p.ContainsFolder( resFolderName ) ) {
				std::cout << "Found resource folder: " << p.ToString() << std::endl;
				resourceFolder = p.ToString() + "\\" + resFolderName;
				foundResourceFolder = true;
				break;
			}
		}
		if ( !foundResourceFolder ) {
			std::cout << "Could not find a resource folder called \"" << resFolderName << "\"!" << std::endl;
			std::exit( EXIT_FAILURE );
		}
	}

	kl::GuiManager::Init();
	kl::Game::Init( scene );
	kl::Input::Init();

	float dt = 0.0f;
	double lastTime = glfwGetTime();
	while ( !glfwWindowShouldClose( g_Window ) ) {
		double now = glfwGetTime();
		dt = static_cast<float>( now - lastTime );
		lastTime = now;

		glfwPollEvents();
		kl::GuiManager::Update();
		kl::Input::Tick();

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		Time::SetDeltaTime( dt );

		// TODO: Separate update and render
		kl::Game::Update();
		kl::GuiManager::Render();

		kl::Input::PostTick();

		glfwSwapBuffers( g_Window );
	}

	kl::Game::Destroy();
	kl::GuiManager::Destroy();

	// Finalize GLFW
	glfwDestroyWindow( g_Window );
	glfwTerminate();

	exit( EXIT_SUCCESS );
}

std::string kl::Application::GetResourceFolder() {
	return resourceFolder;
}
