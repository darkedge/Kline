#pragma once
namespace kl {
class ShaderLoader {
public:
	enum ShaderType {
		VERTEX, FRAGMENT, GEOMETRY,
		TESS_CONTROL, TESS_EVALUATION
	};

	ShaderLoader();

	std::string Log();
	GLuint GetHandle();

	void PrintActiveUniforms();
	void PrintActiveAttribs();

	bool Link();
	bool Validate();

	bool CompileShaderFromFile( const char *fileName, ShaderType type );
	bool CompileShaderFromString( const std::string &source, ShaderType type );

	// Shortcut
	GLuint Compile( const std::string &vs, const std::string &fs );

protected:
	GLuint handle;
	bool linked;
	std::string logString;
};
}
