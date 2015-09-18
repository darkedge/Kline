#pragma once
namespace kl {
class Path {
public:
	Path();
	Path( const std::string &path );
	Path Parent() const;
	std::string ToString() const;
	bool IsFolder() const;
	bool ContainsFile( const std::string &file ) const;
	bool ContainsFolder( const std::string &folder ) const;

	static Path CurrentPath();
	static Path CurrentFolder();

private:
	std::string path;
	std::string name;
};
}
