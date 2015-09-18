#pragma once

namespace kl {
class Material;
class Camera;
class LightManager;

// Performs lazy initialization.
// Materials are not loaded until necessary.
class MaterialManager {
public:
	static void Render( Camera *camera, LightManager *lights );

	template <class T>
	static T *Create() {
		static_assert( std::is_base_of<Material, T>::value, "T is not derived from Material" );
		std::type_index i = typeid( T );

		// Create the material
		T *t = new T();

		// If it's the first one, register it
		if ( materials.count( i ) == 0 ) {
			t->Init();
			t->AddInstance( t );
			materials[i] = t;
		} else {
			// Else, return a copy
			materials.at( i )->AddInstance( t );
		}

		return t;
	}

private:
	static std::unordered_map<std::type_index, Material *> materials;
};
}
