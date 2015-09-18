#pragma once
namespace kl {
class AudioClip {
public:
	AudioClip(const char *path);
	~AudioClip();
private:
	FMOD::Sound *sound;
};
}
