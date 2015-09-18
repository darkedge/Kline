#include "Common.h"
#include "Path.h"

kl::Path::Path() {}

kl::Path::Path( const std::string &path ) : path( path ) {}

kl::Path kl::Path::Parent() const {
	auto a = path.find_last_of( "/\\", path.size() - 2 );
	if ( a == std::string::npos ) {
		return *this;
	}
	return kl::Path( path.substr( 0, a ) );
}

std::string kl::Path::ToString() const {
	return path;
}

kl::Path kl::Path::CurrentPath() {
	char buffer[MAX_PATH];
	GetModuleFileName( nullptr, buffer, MAX_PATH );
	return kl::Path( std::string( buffer ) );
}

kl::Path kl::Path::CurrentFolder() {
	return CurrentPath().Parent();
}

bool kl::Path::IsFolder() const {
	auto attribs = GetFileAttributes( path.c_str() );
	return attribs != INVALID_FILE_ATTRIBUTES
		   && attribs & FILE_ATTRIBUTE_DIRECTORY;
}

bool kl::Path::ContainsFolder( const std::string &folder ) const {
	if ( IsFolder() ) {
		auto str = path + "\\" + folder;
		auto attribs = GetFileAttributes( str.c_str() );
		return attribs != INVALID_FILE_ATTRIBUTES
			   && attribs & FILE_ATTRIBUTE_DIRECTORY;
	} else {
		return Parent().ContainsFolder( folder );
	}
}

bool kl::Path::ContainsFile( const std::string &file ) const {
	if ( IsFolder() ) {
		return klFileExists( path + "\\" + file );
	} else {
		return Parent().ContainsFile( file );
	}
}

#if 0
const char *StripPath(const char *s) {
	std::string str(s);
	size_t t = str.find_last_of("\\/");
	if (t == std::string::npos) {
		return str.c_str();
	}
	else {
		return str.substr(t + 1).c_str();
	}
}
#endif
