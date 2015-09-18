#include "Common.h"
#include "GameObject.h"

#include "Scene.h"

kl::GameObject *kl::GameObject::Find( const char *name ) {
	return scene->Find( name );
}

bool kl::GameObject::IsEnabled() const {
	Component *t = transform.GetParent();

	if ( t ) {
		return enabled && t->GetGameObject()->IsEnabled();
	} else {
		return enabled;
	}
}
