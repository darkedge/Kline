#pragma once
#include "Path.h"

namespace kl {
std::string Resource( const char *file );

class Scene;
class Application {
public:
	static void Start( Scene *scene );
	static std::string GetResourceFolder();
private:
	static std::string resourceFolder;
};
}
