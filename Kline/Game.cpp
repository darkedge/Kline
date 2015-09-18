// TODO: Subsystems do not need to be generalized/templated,
// it looks cool but it does not really serve any purpose

#include "Common.h"
#include "GameObject.h"
#include "Game.h"
#include "MeshRenderer.h"
#include "MeshRendererManager.h"
#include "Behavior.h"
#include "BehaviorManager.h"
#include "LightManager.h"
#include "Scene.h"
#include "Camera.h"
#include "CameraManager.h"
#include "MeshManager.h"
#include "NetworkManager.h"
#include "MaterialManager.h"
#include "MeshRendererManager.h"
#include "AudioSource.h"
#include "AudioManager.h"

using namespace kl;

std::unordered_map<std::type_index, ComponentManager *> Game::subsystems;
Scene *Game::scene;
LightManager *Game::lightManager;
MeshRendererManager *Game::meshRendererManager;
CameraManager *Game::cameraManager;
BehaviorManager *Game::behaviorManager;
AudioManager *Game::audioManager;
int Game::WIDTH = 1024;
int Game::HEIGHT = 768;

void Game::Init( Scene *scene ) {
	// Initialize subsystems
	meshRendererManager = new MeshRendererManager();
	SetSubsystem<MeshRenderer>( meshRendererManager );

	audioManager = new AudioManager();
	SetSubsystem<AudioSource>( audioManager );

	behaviorManager = new BehaviorManager();
	SetSubsystem<Behavior>( behaviorManager );

	cameraManager = new CameraManager();
	SetSubsystem<Camera>( cameraManager );

	MeshManager::Init();

	lightManager = new LightManager();
	SetSubsystem<Light>( lightManager );
	NetworkManager::Init();
	// TODO: NetworkComponent

	// Populate scene
	Game::scene = scene;
	scene->Init();

	// Init behaviors
	behaviorManager->Start();
}

void Game::Update() {

	NetworkManager::Update();

	behaviorManager->Update();

	// Clean up components
	for(auto &pair : subsystems) {
		pair.second->PostTick();
	}

	// Clean up disposed gameobjects
	scene->PostTick();

	audioManager->Update();
	NetworkManager::Send();

	// Render
	cameraManager->Update( lightManager );
}

void Game::Destroy() {
	NetworkManager::Destroy();
	MeshManager::Destroy();
	SAFE_DELETE( meshRendererManager );
	SAFE_DELETE( behaviorManager );
	SAFE_DELETE( cameraManager );
	SAFE_DELETE( scene );
}

void Game::SetResolution( int width, int height ) {
	WIDTH = width;
	HEIGHT = height;
	if ( cameraManager ) {
		cameraManager->RecalculateProjectionMatrices();
	}
}
