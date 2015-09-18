#include "Common.h"
#include "BehaviorManager.h"
#include "Behavior.h"
#include "GameObject.h"

// http://docs.unity3d.com/Manual/ExecutionOrder.html

using namespace kl;

// TODO: This is not how Unity does it
// Start is not called on Behaviors created at runtime
void BehaviorManager::Start() const {
	for ( Behavior *b : behaviors ) {
		b->Start();
	}
}

void BehaviorManager::Update() const {
	for ( Behavior *b : behaviors ) {
		if ( b->IsEnabled() ) {
			b->Update();
		}
	}
}

Component *BehaviorManager::RegisterComponent( Component *component ) {
	behaviors.push_back( static_cast<Behavior *>( component ) );
	return component;
}

// void BehaviorManager::UnregisterComponent( Component *component ) {
// 	STL_ERASE_REMOVE( behaviors, component );
// 	delete component;
// }

void kl::BehaviorManager::PostTick() {
	STL_ERASE_REMOVE_IF( behaviors, []( const Behavior *behavior ) { return behavior->GetGameObject()->IsDisposed();} );
}
