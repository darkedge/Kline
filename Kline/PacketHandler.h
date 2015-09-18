#pragma once
#include "NetworkManager.h"
namespace kl {
class Scene;
class PacketHandler {
	friend class NetworkManager;
public:
	void Init( Scene *scene ) { this->scene = scene; }
	virtual void Connect( const Address &address ) = 0;
	virtual void Timeout( const Address &address ) = 0;
	virtual void DecodePacket( const Address &address, kl::ReadPacket &packet ) = 0;

protected:
	Scene *scene;
};
}
