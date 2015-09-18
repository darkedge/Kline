#pragma once

namespace kl {

class GuiManager;
class ComponentManager;
class MeshRendererManager;
class CameraManager;
class BehaviorManager;
class LightManager;
class AudioManager;
//class ParticleSystem;

class Scene;
class Game {
public:
	static void Init(Scene *scene);
	static void Update();
	static void Destroy();
	static void SetResolution( int width, int height );

	template <class T>
	static ComponentManager *GetSubsystem() {
		try {
			return subsystems.at( std::type_index( typeid( T ) ) );
		} catch ( std::out_of_range ) {
			std::cerr << "No subsystem installed for " << typeid( T ).name() << "!" << std::endl;
			return nullptr;
		}
	}

	static int GetWidth() {
		return WIDTH;
	}

	static int GetHeight() {
		return HEIGHT;
	}

protected:
	template <class T>
	static void SetSubsystem( ComponentManager *subsystem ) {
		subsystems[std::type_index( typeid( T ) )] = subsystem;
	}

	static std::unordered_map<std::type_index, ComponentManager *> subsystems;

	static Scene *scene;
	static int WIDTH, HEIGHT;

	static MeshRendererManager *meshRendererManager;
	static CameraManager *cameraManager;
	static BehaviorManager *behaviorManager;
	static LightManager *lightManager;
	static AudioManager *audioManager;
};
} // namespace mj
