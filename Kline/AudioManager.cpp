#include "Common.h"
#include <fmod_errors.h>
#include "AudioManager.h"
#include "AudioSource.h"
#include "GameObject.h"

// Init
kl::AudioManager::AudioManager() {
	FMOD_TRY( FMOD::System_Create( &system ) );  // Create the main system object.
	FMOD_TRY( system->init( 512, FMOD_INIT_NORMAL, 0 ) ); // Initialize FMOD.
}

kl::AudioManager::~AudioManager() {
	system->release();
}

kl::Component *kl::AudioManager::RegisterComponent( kl::Component *component ) {
	audioSources.push_back( ( AudioSource * )component );
	return component;
}

// void kl::AudioManager::UnregisterComponent( kl::Component *component ) {
// 	STL_ERASE_REMOVE( audioSources, component );
// 	delete component;
// }

void kl::AudioManager::Update() const {
	//KL_FMOD_TRY(system->set3DListenerAttributes(0, &listener_pos, &listener_vel, &listener_forward, &listener_up));     // update 'ears'
	FMOD_TRY( system->update() );
}

void kl::AudioManager::PostTick() {
	STL_ERASE_REMOVE_IF( audioSources, []( const AudioSource *audioSource ) { return audioSource->GetGameObject()->IsDisposed();} );
}
