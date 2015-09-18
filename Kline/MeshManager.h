#pragma once

#if 1 // OpenGL V flip
#define KL_V(t) (1 - (t))
#else
#define KL_V(t) t
#endif

namespace kl {
class Mesh;

// TODO: Implement tangent space? Perhaps create a seperate buffer for this vertex format
// TODO: There is no "delete mesh" function. Because I am using a single
//			VBO, deleting subdata means reallocating the entire buffer.
class MeshManager {
public:
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoord;
	};

	static void Init();
	static void Destroy();
	static void Bind();
	static void DrawMesh( int i );
	static int CreateMesh( const std::vector<Vertex> &vertices, const std::vector<int> &indices );

	static int Quad() {
		return quad;
	}
	static int Cube() {
		return cube;
	}

	static int CreateSphere( int slices = 20, int stacks = 20, float radius = 0.5f );
	static int CreateCylinder( int slices = 20, float radius = 0.5f, float height = 1.0f );
	static int CreateTorus( float diameter = 0.75f, float thickness = 0.25f, int slices = 20 );

private:
	static void CreateQuad();
	static void CreateCube();

	struct Mesh {
		int indexDataOffsetBytes;
		int baseVertex;
		int indexCount;
	};
	static int quad;
	static int cube;
	static std::vector<Mesh> meshes;
	static GLuint vao;
	static GLuint vertexBuffer;
	static GLuint indexBuffer;
	static GLintptr vertexCount;
	static GLintptr indexCount;
};
}
