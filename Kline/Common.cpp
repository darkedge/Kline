#include "Common.h"

bool klFileExists( const std::string &filename ) {
	struct stat buf;
	return stat( filename.c_str(), &buf ) == 0;
}

_off_t klFileSize( const std::string &filename ) {
	struct stat buf;
	stat( filename.c_str(), &buf );
	return buf.st_size;
}
