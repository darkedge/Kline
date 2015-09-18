#include "Common.h"
#include "Input.h"

using namespace kl;

// Keyboard
bool Input::prev[KL_NUM_KEYBOARD_KEYS];
bool Input::keys[KL_NUM_KEYBOARD_KEYS];
bool Input::down[KL_NUM_KEYBOARD_KEYS];
bool Input::up[KL_NUM_KEYBOARD_KEYS];

// Mouse
bool Input::mousePrev[KL_NUM_MOUSE_BUTTONS];
bool Input::mouseButtons[KL_NUM_MOUSE_BUTTONS];
bool Input::mouseDown[KL_NUM_MOUSE_BUTTONS];
bool Input::mouseUp[KL_NUM_MOUSE_BUTTONS];

bool Input::isMouseGrabbed;
glm::vec2 Input::lastMousePos;
glm::vec2 Input::mouseDelta;
glm::vec2 Input::mousePos;
