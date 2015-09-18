#pragma once
#include "ComponentManager.h"

namespace kl {
class AudioSource;

class AudioManager : public ComponentManager {
public:
	AudioManager();
	~AudioManager();

	Component *RegisterComponent( Component *component ) override;
	//void UnregisterComponent( Component *component ) override;
	void PostTick() override;

	void Update() const;

private:
	std::vector<AudioSource *> audioSources;
	FMOD::System *system = nullptr;	
};
}
