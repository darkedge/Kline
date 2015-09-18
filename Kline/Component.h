#pragma once

namespace kl {
class GameObject;

class Component {
public:
	Component( GameObject *gameObject ) : gameObject( gameObject ) {}

	GameObject *GetGameObject() const {
		return gameObject;
	}

	void SetEnabled( bool enabled ) { this->enabled = enabled; }
	bool IsEnabled() const;

	Component &operator=( const Component & ) = delete;

protected:
	GameObject *const gameObject;

private:
	Component();
	bool enabled = true;
};
} // namespace kl
