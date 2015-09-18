#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

// Standard library
#include <algorithm>
#include <bitset>
#include <cctype>
#include <chrono>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>

// Networking
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Winsock2.h>
#include <WS2tcpip.h>

// OpenCL
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
//#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/opencl.h>

// FMOD
#include <fmod.hpp>

// glad
#include <glad/glad.h>

// ImGui
#include "imgui.h"

// GLFW
#include <GLFW/glfw3.h>
//#include <GLFW/glfw3native.h>

// FreeType
#include <ft2build.h>
#include <freetype.h>
#include <ftglyph.h>

// GLM
//#define GLM_MESSAGES
#define GLM_FORCE_RADIANS
#pragma warning( push )
#pragma warning( disable: 4201 )
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>			// Handles the interaction between pointers and vector, matrix types
#include <glm/gtx/euler_angles.hpp>		// Build matrices from Euler angles
#include <glm/gtx/quaternion.hpp>		// Extended quaternion types and functions
#include <glm/gtx/transform.hpp>		// Add transformation matrices
#pragma warning( pop )

// Assimp
#include <assimp/Importer.hpp>

bool klFileExists( const std::string &filename );
_off_t klFileSize( const std::string &filename );

#define ZERO_MEM(a) memset( &a, 0, sizeof(a) )
#define KLINE_STATIC_ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define SAFE_DELETE(p) { delete p; p = nullptr; }

#define STL_CONTAINS(c, x) ( std::find( std::begin(c), std::end(c), x) != std::end(c) )
#define STL_ERASE_REMOVE(c, v) { c.erase(std::remove(std::begin(c), std::end(c), v), std::end(c)); }
#define STL_ERASE_REMOVE_IF(c, p) { c.erase(std::remove_if(std::begin(c), std::end(c), p), std::end(c)); }

// Stringify operators
#define XSTR(X) STR(X)
#define STR(X) #X

extern GLFWwindow *g_Window;

#define CL_TRY(expr)\
{\
	cl_int err = expr;\
	if (err != CL_SUCCESS)\
	{\
		printf("%s(%d): %s failed, OpenCL error: %d\n", __FILE__, __LINE__, #expr, err);\
	}\
}

#define CL_CHECK(err)\
{\
	if (err != CL_SUCCESS)\
	{\
		printf("%s(%d): OpenCL error: %d\n", __FILE__, __LINE__, err);\
	}\
}

#define FMOD_TRY(expr)\
{\
	FMOD_RESULT result = expr;\
	if (result != FMOD_OK) {\
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));\
	}\
}

#pragma warning( disable: 4316 ) //object allocated on the heap may not be aligned 16
