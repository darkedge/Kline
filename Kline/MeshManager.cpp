#include "Common.h"
#include "MeshManager.h"

using namespace kl;

GLuint MeshManager::vao;
GLuint MeshManager::indexBuffer;
GLuint MeshManager::vertexBuffer;
GLintptr MeshManager::vertexCount;
GLintptr MeshManager::indexCount;
std::vector<MeshManager::Mesh> MeshManager::meshes;
int MeshManager::quad;
int MeshManager::cube;

static const float PI = glm::pi<float>();
static const float TAU = 2.0f * PI;

void MeshManager::Init() {
	vertexCount = 0;
	indexCount = 0;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	glGenBuffers( 1, &vertexBuffer );
	glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer );

	// TODO: See https://www.opengl.org/wiki/Vertex_Specification_Best_Practices
	// For vertex attribute size optimizations

	// Allocate 4 MB vertex attribute buffer without setting data
	glBufferData( GL_ARRAY_BUFFER, 4 * 1024 * 1024, nullptr, GL_STATIC_DRAW );
	// Tell OpenGL where we can find vertex attributes
	// Position
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), nullptr );
	glEnableVertexAttribArray( 0 );
	// Normal
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( void * )12 );
	glEnableVertexAttribArray( 1 );
	// Texcoord
	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( void * )24 );
	glEnableVertexAttribArray( 2 );

	// Index buffer (also 4 MB, probably too big)
	glGenBuffers( 1, &indexBuffer );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * 1024 * 1024, nullptr, GL_STATIC_DRAW );

	// Unbind
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// Create basic shapes
	CreateQuad();
	CreateCube();
}

void MeshManager::CreateQuad() {
	std::vector<Vertex> vertices = {
		{ { -0.5, -0.5, 0 }, { 0, 0, 1 }, { 0, KL_V(0) } },
		{ { 0.5, -0.5, 0 }, { 0, 0, 1 }, { 1, KL_V(0) } },
		{ { 0.5, 0.5, 0 }, { 0, 0, 1 }, { 1, KL_V(1) } },
		{ { -0.5, 0.5, 0 }, { 0, 0, 1 }, { 0, KL_V(1) } },
	};

	std::vector<int> indices = { 0, 1, 2, 2, 3, 0 };

	quad = CreateMesh( vertices, indices );
}

void MeshManager::CreateCube() {
	std::vector<Vertex> vertices = {
		//
		{ { -0.5, -0.5, -0.5 }, { -1, 0, 0 }, { 0, KL_V(0) } }, // 0
		{ { -0.5, -0.5, -0.5 }, { 0, -1, 0 }, { 0, KL_V(0) } }, // 1
		{ { -0.5, -0.5, -0.5 }, { 0, 0, -1 }, { 1, KL_V(0) } }, // 2

		{ { -0.5, -0.5, 0.5 }, { -1, 0, 0 }, { 1, KL_V(0) } }, // 3
		{ { -0.5, -0.5, 0.5 }, { 0, -1, 0 }, { 0, KL_V(1) } }, // 4
		{ { -0.5, -0.5, 0.5 }, { 0, 0, 1 }, { 0, KL_V(0) } }, // 5

		{ { -0.5, 0.5, -0.5 }, { -1, 0, 0 }, { 0, KL_V(1) } }, // 6
		{ { -0.5, 0.5, -0.5 }, { 0, 0, -1 }, { 1, KL_V(1) } }, // 7
		{ { -0.5, 0.5, -0.5 }, { 0, 1, 0 }, { 0, KL_V(1) } }, // 8

		{ { -0.5, 0.5, 0.5 }, { -1, 0, 0 }, { 1, KL_V(1) } }, // 9
		{ { -0.5, 0.5, 0.5 }, { 0, 0, 1 }, { 0, KL_V(1) } }, // 10
		{ { -0.5, 0.5, 0.5 }, { 0, 1, 0 }, { 0, KL_V(0) } }, // 11

		{ { 0.5, -0.5, -0.5 }, { 0, -1, 0 }, { 1, KL_V(0) } }, // 12
		{ { 0.5, -0.5, -0.5 }, { 0, 0, -1 }, { 0, KL_V(0) } }, // 13
		{ { 0.5, -0.5, -0.5 }, { 1, 0, 0 }, { 1, KL_V(0) } }, // 14

		{ { 0.5, -0.5, 0.5 }, { 0, -1, 0 }, { 1, KL_V(1) } }, // 15
		{ { 0.5, -0.5, 0.5 }, { 0, 0, 1 }, { 1, KL_V(0) } }, // 16
		{ { 0.5, -0.5, 0.5 }, { 1, 0, 0 }, { 0, KL_V(0) } }, // 17

		{ { 0.5, 0.5, -0.5 }, { 0, 0, -1 }, { 0, KL_V(1) } }, // 18
		{ { 0.5, 0.5, -0.5 }, { 0, 1, 0 }, { 1, KL_V(1) } }, // 19
		{ { 0.5, 0.5, -0.5 }, { 1, 0, 0 }, { 1, KL_V(1) } }, // 20

		{ { 0.5, 0.5, 0.5 }, { 0, 0, 1 }, { 1, KL_V(1) } }, // 21
		{ { 0.5, 0.5, 0.5 }, { 0, 1, 0 }, { 1, KL_V(0) } }, // 22
		{ { 0.5, 0.5, 0.5 }, { 1, 0, 0 }, { 0, KL_V(1) } }, // 23
	};

	std::vector<int> indices = {
		5, 16, 10, 10, 16, 21,
		11, 22, 8, 8, 22, 19,
		7, 18, 2, 2, 18, 13,
		1, 12, 4, 4, 12, 15,
		17, 14, 23, 23, 14, 20,
		0, 3, 6, 6, 3, 9,
	};

	cube = CreateMesh( vertices, indices );
}

int MeshManager::CreateSphere( int slices, int stacks, float radius ) {
	std::vector<Vertex> vertices;
	std::vector<int> indices;

	for ( int i = 0; i <= stacks; ++i ) {
		// V texture coordinate.
		float v = i / static_cast<float>( stacks );
		float phi = v * PI;

		for ( int j = 0; j <= slices; ++j ) {
			// U texture coordinate.
			float u = j / static_cast<float>( slices );
			float theta = u * TAU;

			float x = cos( theta ) * sin( phi );
			float y = cos( phi );
			float z = sin( theta ) * sin( phi );

			vertices.push_back( {
				glm::vec3( x, y, z ) * radius,
				glm::vec3( x, y, z ),
				glm::vec2( 1.0f - u, KL_V(1.0f - v) )
			} );
		}
	}

	for ( int i = 0; i < slices * stacks + slices; ++i ) {
		indices.insert( indices.end(), {
			i, i + slices + 1, i + slices,
			i + slices + 1, i, i + 1
		} );
	}

	return CreateMesh( vertices, indices );
}

int MeshManager::CreateCylinder( int slices, float radius, float height ) {
	std::vector<Vertex> vertices;
	std::vector<int> indices;

	// Vertices
	for ( int j = 0; j <= slices; ++j ) {
		// U texture coordinate.
		float r = j / static_cast<float>( slices );
		float theta = r * TAU;

		// Texture tiling
		const int TILING = 2;
		float u = TILING * r;

		float x = cos( theta );
		float z = sin( theta );

		vertices.insert( vertices.end(), {
			{
				// Top
				glm::vec3( x * radius, height / 2, z * radius ),
				glm::vec3( 0, 1, 0 ),
				glm::vec2( x / 2 + 0.5f, KL_V(1.0f - (z / 2 + 0.5f)) )
			},
			{
				// Top side
				glm::vec3( x * radius, height / 2, z * radius ),
				glm::vec3( x, 0, z ),
				glm::vec2( 1 - u, KL_V(1) )
			},
			{
				// Bottom side
				glm::vec3( x * radius, -height / 2, z * radius ),
				glm::vec3( x, 0, z ),
				glm::vec2( 1 - u, KL_V(0) )
			},
			{
				// Bottom
				glm::vec3( x * radius, -height / 2, z * radius ),
				glm::vec3( 0, -1, 0 ),
				glm::vec2( x / 2 + 0.5f, KL_V(z / 2 + 0.5f) )
			}
		} );
	}

	// Top center point
	int topIndex = (int) vertices.size();
	vertices.push_back( {
		glm::vec3( 0, height / 2, 0 ),
		glm::vec3( 0, 1, 0 ),
		glm::vec2( 0.5f, 0.5f )
	} );

	// Bottom center point
	int bottomIndex = (int) vertices.size();
	vertices.push_back( {
		glm::vec3( 0, -height / 2, 0 ),
		glm::vec3( 0, -1, 0 ),
		glm::vec2( 0.5f, 0.5f )
	} );

	// Indices
	for ( int i = 0; i < slices; i++ ) {
		int j = 4 * i;

		indices.insert( indices.end(), {
			j, topIndex, j + 4,			// Top
			j + 3, j + 7, bottomIndex,	// Bottom
			j + 2, j + 1, j + 5,		// Side
			j + 5, j + 6, j + 2			// Side
		} );
	}

	return CreateMesh( vertices, indices );
}

int MeshManager::CreateTorus( float diameter, float thickness, int slices ) {
	std::vector<Vertex> vertices;
	std::vector<int> indices;

	assert( slices >= 3 );

	auto stride = slices + 1;

	// Toroidal direction
	for ( int i = 0; i <= slices; i++ ) {
		// Texture tiling
		const float TILING = 2.0f;
		float u = TILING * ( float ) i / slices;

		float outerAngle = i * TAU / slices - PI / 2;

		// Create a transform matrix that will align geometry to
		// slice perpendicularly though the current ring position.
		glm::mat4 transform = glm::eulerAngleZ( outerAngle ) * glm::translate( glm::vec3( diameter / 2, 0, 0 ) );

		// Poloidal direction
		for ( int j = 0; j <= slices; j++ ) {
			float v = 1 - ( float ) j / slices;

			float innerAngle = j * TAU / slices + PI;
			float dx = cos( innerAngle );
			float dy = sin( innerAngle );

			// Create a vertex.
			glm::vec4 normal( dx, 0, dy, 0 );
			glm::vec4 position( glm::vec3( normal * thickness / 2.0f ), 1.0f );
			glm::vec2 textureCoordinate( u, KL_V(v) );

			position = transform * position;
			normal = transform * normal;

			vertices.push_back( {
				glm::vec3( position ),
				glm::vec3( normal ),
				textureCoordinate
			} );

			// And create indices for two triangles.
			int nextI = ( i + 1 ) % stride;
			int nextJ = ( j + 1 ) % stride;

			indices.insert( indices.end(), {
				i * stride + j,
				nextI * stride + j,
				i * stride + nextJ,

				i * stride + nextJ,
				nextI * stride + j,
				nextI * stride + nextJ,
			} );
		}
	}

	return CreateMesh( vertices, indices );
}

void MeshManager::Destroy() {
	glDeleteBuffers( 1, &vertexBuffer );
	glDeleteBuffers( 1, &indexBuffer );
	glDeleteVertexArrays( 1, &vao );
}

// FIXME: Buffers will be full sometime
int MeshManager::CreateMesh( const std::vector<Vertex> &vertices, const std::vector<int> &indices ) {
	const int mesh = (int) meshes.size();
	const int *indexPtr = (int*)indices.data();

	glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, vertexCount * sizeof( Vertex ), vertices.size() * sizeof( Vertex ), vertices.data() );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer );
	glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof( int ), indices.size() * sizeof( int ), indexPtr );

	meshes.push_back( { (int) indexCount * sizeof( int ), (int)vertexCount, (int) indices.size() } );
	vertexCount += vertices.size();
	indexCount += indices.size();

	return mesh;
}

void MeshManager::Bind() {
	glBindVertexArray( vao );
}

void MeshManager::DrawMesh( int index ) {
	const Mesh &i = meshes[index];
	glDrawElementsBaseVertex(
		GL_TRIANGLES,
		i.indexCount,
		GL_UNSIGNED_INT,
		( void * )( i.indexDataOffsetBytes ),
		i.baseVertex );
}
