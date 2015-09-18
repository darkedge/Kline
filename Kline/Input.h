#pragma once

#define KL_NUM_KEYBOARD_KEYS GLFW_KEY_LAST + 1
#define KL_NUM_MOUSE_BUTTONS GLFW_MOUSE_BUTTON_LAST + 1

namespace kl {
class Input {
public:
	static bool GetKey( int key ) {
		return keys[key];
	}
	static bool GetKeyDown( int key ) {
		return down[key];
	}
	static bool GetKeyUp( int key ) {
		return up[key];
	}

	static bool GetMouse( int button ) {
		return mouseButtons[button];
	}

	static bool GetMouseDown( int button ) {
		return mouseDown[button];
	}

	static bool GetMouseUp( int button ) {
		return mouseUp[button];
	}

	static bool IsMouseGrabbed() {
		return isMouseGrabbed;
	}

	// In pixels. Origin is top-left
	static glm::vec2 GetMouseDelta() {
		return mouseDelta;
	}

	// In pixels. Origin is top-left
	static glm::vec2 GetMousePosition() {
		return mousePos;
	}

	static void SetMouseButton( int button, bool pressed ) {
		mouseButtons[button] = pressed;
	}

	static void SetKey( int key, bool pressed ) {
		keys[key] = pressed;
	}

	static void SetMousePosition( const glm::vec2 pos ) {
		mousePos = pos;
		mouseDelta += pos - lastMousePos;
	}

	static void Init() {
		// Keyboard
		memset( prev, int( false ), KL_NUM_KEYBOARD_KEYS * sizeof( bool ) );
		memset( keys, int( false ), KL_NUM_KEYBOARD_KEYS * sizeof( bool ) );
		memset( down, int( false ), KL_NUM_KEYBOARD_KEYS * sizeof( bool ) );
		memset( up, int( false ), KL_NUM_KEYBOARD_KEYS * sizeof( bool ) );

		// Mouse
		memset( mousePrev, int( false ), KL_NUM_MOUSE_BUTTONS * sizeof( bool ) );
		memset( mouseButtons, int( false ), KL_NUM_MOUSE_BUTTONS * sizeof( bool ) );
		memset( mouseDown, int( false ), KL_NUM_MOUSE_BUTTONS * sizeof( bool ) );
		memset( mouseUp, int( false ), KL_NUM_MOUSE_BUTTONS * sizeof( bool ) );

		isMouseGrabbed = false;
		mouseDelta = {0, 0};
	}

	static void Tick() {
		// Keyboard
		{
			bool changes[KL_NUM_KEYBOARD_KEYS];
			for ( unsigned int i = 0; i < KL_NUM_KEYBOARD_KEYS; i++ ) {
				changes[i]	= keys[i]		^ prev[i];
				down[i]		= changes[i]	& keys[i];
				up[i]		= changes[i]	& !keys[i];
			}
		}

		// Mouse
		{
			bool changes[KL_NUM_MOUSE_BUTTONS];
			for ( unsigned int i = 0; i < KL_NUM_MOUSE_BUTTONS; i++ ) {
				changes[i] = mouseButtons[i] ^ mousePrev[i];
				mouseDown[i] = changes[i] & mouseButtons[i];
				mouseUp[i] = changes[i] & !mouseButtons[i];
			}
		}
	}

	static void PostTick() {
		memcpy( prev, keys, KL_NUM_KEYBOARD_KEYS * sizeof( bool ) );
		memcpy( mousePrev, mouseButtons, KL_NUM_MOUSE_BUTTONS * sizeof( bool ) );
		mouseDelta = {0, 0};
		lastMousePos = mousePos;
	}

	static void SetMouseGrabbed( bool grabMouse ) {
		isMouseGrabbed = grabMouse;
		if ( isMouseGrabbed ) {
			glfwSetInputMode( g_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
		} else {
			glfwSetInputMode( g_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
		}
	}

protected:
	// Keyboard
	static bool prev[KL_NUM_KEYBOARD_KEYS];
	static bool keys[KL_NUM_KEYBOARD_KEYS];
	static bool down[KL_NUM_KEYBOARD_KEYS];
	static bool up[KL_NUM_KEYBOARD_KEYS];

	// Mouse
	static bool mousePrev[KL_NUM_MOUSE_BUTTONS];
	static bool mouseButtons[KL_NUM_MOUSE_BUTTONS];
	static bool mouseDown[KL_NUM_MOUSE_BUTTONS];
	static bool mouseUp[KL_NUM_MOUSE_BUTTONS];

	static bool isMouseGrabbed;
	static glm::vec2 lastMousePos;
	static glm::vec2 mouseDelta;
	static glm::vec2 mousePos;
};
} // namespace mj
