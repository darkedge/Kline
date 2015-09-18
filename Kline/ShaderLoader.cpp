#include "Common.h"
#include "ShaderLoader.h"

using namespace kl;

ShaderLoader::ShaderLoader() : handle( 0 ), linked( false ) { }

bool ShaderLoader::CompileShaderFromFile( const char *fileName, ShaderType type ) {
	if ( !klFileExists( fileName ) ) {
		logString = "File not found.";
		return false;
	}

	if ( handle <= 0 ) {
		handle = glCreateProgram();
		if ( handle == 0 ) {
			logString = "Unable to create shader program.";
			return false;
		}
	}

	std::ifstream inFile( fileName, std::ios::in );
	if ( !inFile ) {
		return false;
	}

	std::ostringstream code;
	while ( inFile.good() ) {
		int c = inFile.get();
		if ( ! inFile.eof() )
			code << ( char ) c;
	}
	inFile.close();

	return CompileShaderFromString( code.str(), type );
}

bool ShaderLoader::CompileShaderFromString( const std::string &source, ShaderLoader::ShaderType type ) {
	if ( handle <= 0 ) {
		handle = glCreateProgram();
		if ( handle == 0 ) {
			logString = "Unable to create shader program.";
			return false;
		}
	}

	GLuint shaderHandle = 0;

	switch ( type ) {
	case ShaderLoader::VERTEX:
		shaderHandle = glCreateShader( GL_VERTEX_SHADER );
		break;
	case ShaderLoader::FRAGMENT:
		shaderHandle = glCreateShader( GL_FRAGMENT_SHADER );
		break;
	case ShaderLoader::GEOMETRY:
		shaderHandle = glCreateShader( GL_GEOMETRY_SHADER );
		break;
	case ShaderLoader::TESS_CONTROL:
		shaderHandle = glCreateShader( GL_TESS_CONTROL_SHADER );
		break;
	case ShaderLoader::TESS_EVALUATION:
		shaderHandle = glCreateShader( GL_TESS_EVALUATION_SHADER );
		break;
	default:
		return false;
	}

	const char *c_code = source.c_str();
	glShaderSource( shaderHandle, 1, &c_code, nullptr );

	// Compile the shader
	glCompileShader( shaderHandle );

	// Check for errors
	int result;
	glGetShaderiv( shaderHandle, GL_COMPILE_STATUS, &result );
	if ( GL_FALSE == result ) {
		// Compile failed, store log and return false
		int length = 0;
		logString = "";
		glGetShaderiv( shaderHandle, GL_INFO_LOG_LENGTH, &length );
		if ( length > 0 ) {
			char *c_log = new char[length];
			int written = 0;
			glGetShaderInfoLog( shaderHandle, length, &written, c_log );
			logString = c_log;
			delete [] c_log;
		}

		return false;
	} else {
		// Compile succeeded, attach shader and return true
		glAttachShader( handle, shaderHandle );
		return true;
	}
}

bool ShaderLoader::Link() {
	if ( linked ) return true;
	if ( handle <= 0 ) {
		logString = "";
		return false;
	}

	glLinkProgram( handle );

	int status = 0;
	glGetProgramiv( handle, GL_LINK_STATUS, &status );
	if ( GL_FALSE == status ) {
		// Store log and return false
		int length = 0;
		logString = "";

		glGetProgramiv( handle, GL_INFO_LOG_LENGTH, &length );

		if ( length > 0 ) {
			char *c_log = new char[length];
			int written = 0;
			glGetProgramInfoLog( handle, length, &written, c_log );
			logString = c_log;
			delete [] c_log;
		}

		return false;
	} else {
		linked = true;
		return linked;
	}
}

std::string ShaderLoader::Log() {
	return logString;
}

GLuint ShaderLoader::GetHandle() {
	return handle;
}

void ShaderLoader::PrintActiveUniforms() {

	GLint nUniforms, size, location, maxLen;
	GLchar *name;
	GLsizei written;
	GLenum type;

	glGetProgramiv( handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen );
	glGetProgramiv( handle, GL_ACTIVE_UNIFORMS, &nUniforms );

	name = ( GLchar * ) malloc( maxLen );

	printf( " Location | Name\n" );
	printf( "------------------------------------------------\n" );
	for ( int i = 0; i < nUniforms; ++i ) {
		glGetActiveUniform( handle, i, maxLen, &written, &size, &type, name );
		location = glGetUniformLocation( handle, name );
		printf( " %-8d | %s\n", location, name );
	}

	free( name );
}

void ShaderLoader::PrintActiveAttribs() {

	GLint written, size, location, maxLength, nAttribs;
	GLenum type;
	GLchar *name;

	glGetProgramiv( handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength );
	glGetProgramiv( handle, GL_ACTIVE_ATTRIBUTES, &nAttribs );

	name = ( GLchar * ) malloc( maxLength );

	printf( " Index | Name\n" );
	printf( "------------------------------------------------\n" );
	for ( int i = 0; i < nAttribs; i++ ) {
		glGetActiveAttrib( handle, i, maxLength, &written, &size, &type, name );
		location = glGetAttribLocation( handle, name );
		printf( " %-5d | %s\n", location, name );
	}

	free( name );
}

bool ShaderLoader::Validate() {
	if ( !linked ) {
		logString = "";
		return false;
	}

	GLint status;
	glValidateProgram( handle );
	glGetProgramiv( handle, GL_VALIDATE_STATUS, &status );

	if ( GL_FALSE == status ) {
		// Store log and return false
		int length = 0;
		logString = "";

		glGetProgramiv( handle, GL_INFO_LOG_LENGTH, &length );

		if ( length > 0 ) {
			char *c_log = new char[length];
			int written = 0;
			glGetProgramInfoLog( handle, length, &written, c_log );
			logString = c_log;
			delete [] c_log;
		}

		return false;
	} else {
		return true;
	}
}

GLuint ShaderLoader::Compile( const std::string &vs, const std::string &fs ) {
	if ( !CompileShaderFromFile( vs.c_str(), VERTEX ) ) {
		printf( "ERROR: %s - %s\n", vs.c_str(), Log().c_str() );
	}
	if ( !CompileShaderFromFile( fs.c_str(), FRAGMENT ) ) {
		printf( "ERROR: %s - %s\n", fs.c_str(), Log().c_str() );
	}
	if ( !Link() ) {
		printf( "Shader program failed to link! %s\n", Log().c_str() );
	}
	if ( !Validate() ) {
		printf( "Program failed to validate! %s\n", Log().c_str() );
	}
	return handle;
}
