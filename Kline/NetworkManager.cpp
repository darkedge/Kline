#include "Common.h"
#include "NetworkManager.h"
#include "MTime.h"
#include "PacketHandler.h"

using namespace kl;

Connection NetworkManager::connection;
bool NetworkManager::connected;
PacketHandler *NetworkManager::packetHandler;
float NetworkManager::time = 0.0f;
bool NetworkManager::shouldSend = false;
int64_t NetworkManager::numBytesSentInclusive = 0;
int64_t NetworkManager::numBytesSentExclusive = 0;

static const unsigned int PROTOCOL_ID = 0x5E21C6DB;
static const float TIMEOUT = 10.0f;
static const int PORT = 22223;
static const float TICK_TIME = 1.0f / 33.0f;

// TODO: Tickrate
void NetworkManager::Init() {

	// Initialize sockets
	WSADATA WsaData;
	int errCode = WSAStartup( MAKEWORD( 2, 2 ), &WsaData );
	assert( errCode == NO_ERROR );

	connection = Connection( PROTOCOL_ID, TIMEOUT );
}

void WritePacket::WriteHeader() {
	assert( writePos == 0 );

	// TODO: More header stuff
	WriteU32( PROTOCOL_ID );
}


void NetworkManager::StartServer() {
	if ( connection.Start( PORT ) ) {
		printf( "Created server on port %d.\n", PORT );
	} else {
		printf( "Failed to create server on port %d!\n", PORT );
		return;
	}

	connection.Listen();
	printf( "Now listening for incoming connections.\n" );
}

void NetworkManager::StartClient( const char *ip ) {
#if 0 // TODO: If we ever get to doing DNS lookup
	addrinfo *result = NULL;
	addrinfo *ptr = NULL;
	addrinfo hints;
	ZeroMemory( &hints, sizeof( hints ) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	auto retval = getaddrinfo( ip, "1112", &hints, &result );
#endif

	printf( "Connecting to \"%s\"...\n", ip );

	unsigned long addr;
	if ( inet_pton( AF_INET, ip, &addr ) != 1 ) {
		printf( "Invalid ip format!\n" );
		return;
	}

	if ( !connection.Start( 0 ) ) {
		printf( "could not start connection on random port\n" );
		return;
	}

	connection.Connect( Address( htonl( addr ), PORT ) );
	connected = false;

	// Send packet to say hello
	connection.ClearPackets();
	connection.SendPackets();
}

void NetworkManager::Destroy() {
	// Shut down sockets
	WSACleanup();
}

// TODO: Authority is not the right word
// Authority can be per object
bool NetworkManager::IsServer() {
	return connection.GetMode() == Connection::Server;
}

/************************************************************************/
/* Update                                                               */
/************************************************************************/
void NetworkManager::Update() {
	// Prep packet for receiving
	ReadPacket recvPacket;
	Address address;
	switch ( connection.GetMode() ) {
	case Connection::Server:

		for ( ;; ) {
			recvPacket.Clear();
			int bytes_read = connection.ReceivePacket( address, recvPacket );
			if ( bytes_read == 0 )
				break;
			packetHandler->DecodePacket( address, recvPacket );
		}

		connection.Update( Time::GetDeltaTime() );
		break;
	case Connection::Client:
		if ( !connected && connection.IsConnected() ) {
			printf( "client connected to server\n" );
			connected = true;
		}

		if ( !connected && connection.ConnectFailed() ) {
			printf( "connection failed\n" );
			connection.Stop();
			break;
		}

		for ( ;; ) {
			recvPacket.Clear();
			int bytes_read = connection.ReceivePacket( address, recvPacket );
			if ( bytes_read == 0 )
				break;
			packetHandler->DecodePacket( address, recvPacket );
		}

		connection.Update( Time::GetDeltaTime() );
		break;
	}
}

void NetworkManager::Send() {
	time += Time::GetDeltaTime();
	if ( time >= TICK_TIME ) {
		time -= TICK_TIME;

		connection.SendPackets();

		// Prep packets for sending
		connection.ClearPackets();
	}
}

bool kl::Socket::Send( const Address &destination, const WritePacket &packet )  {
	assert( packet.Size() > 0 );

	if ( socket == 0 )
		return false;

	assert( destination.GetAddress() != 0 );
	assert( destination.GetPort() != 0 );

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl( destination.GetAddress() );
	address.sin_port = htons( ( unsigned short ) destination.GetPort() );

	int sent_bytes = sendto( socket, packet.Data(), packet.Size(), 0, ( sockaddr * ) &address, sizeof( sockaddr_in ) );
	kl::NetworkManager::IncrementBytesSentInclusive( sent_bytes );
	kl::NetworkManager::IncrementBytesSentExclusive( sent_bytes - 4 );

	return sent_bytes == packet.Size();
}

void kl::Connection::Update( float deltaTime ) {
	assert( running );

	switch ( mode ) {
	case Client:
		timeoutAccumulator += deltaTime;
		if ( timeoutAccumulator > timeout ) {
			if ( state == Connecting ) {
				printf( "Network: connect timed out\n" );
				ClearData();
				state = ConnectFail;
			} else if ( state == Connected ) {
				printf( "Network: connection timed out\n" );
				ClearData();
				if ( state == Connecting ) {
					state = ConnectFail;
				}
				if ( packetHandler ) {
					packetHandler->Timeout( address );
				}
			}
		}
		break;
	case Server: {
		auto pm_it = recipients.begin();
		while ( pm_it != recipients.end() ) {
			auto &client = pm_it->second;
			auto &addr = pm_it->first;
			client.timeoutAccumulator += deltaTime;
			assert( client.state != Connecting );
			if ( client.timeoutAccumulator > timeout ) {
				// No response after trying to connect
				if ( client.state == Connecting ) {
					printf( "Network: %d.%d.%d.%d:%d connect timed out\n",
							addr.GetA(), addr.GetB(), addr.GetC(), addr.GetD(), addr.GetPort() );
					if ( packetHandler ) {
						packetHandler->Timeout( addr );
					}
					pm_it = recipients.erase( pm_it );
					continue;
				}
				// Lost connection
				if ( client.state == Connected ) {
					printf( "Network: %d.%d.%d.%d:%d connection timed out\n",
							addr.GetA(), addr.GetB(), addr.GetC(), addr.GetD(), addr.GetPort() );
					if ( packetHandler ) {
						packetHandler->Timeout( addr );
					}
					pm_it = recipients.erase( pm_it );
					continue;
				}
			}
			++pm_it;
		}
		break;
	}
	default:
		break;
	}
}

int kl::Connection::ReceivePacket( Address &sender, ReadPacket &packet ) {
	assert( running );
	int bytes_read = socket.Receive( sender, packet );
	// 		if ( bytes_read == 0 )
	// 			return bytes_read;
	if ( bytes_read < 4 ) // FIXME: Header size is constant
		return 0;

	// Read header
	if ( packet.ReadU32() != protocolId ) {
		// protocol id mismatch
		return 0;
	}

	// Client connected (server-side)
	if ( mode == Server ) {
		// Silently adds new recipient if this is the first time
		recipients[sender].timeoutAccumulator = 0.0f;
		if ( recipients[sender].state != Connected ) {
			printf( "Network: server accepts connection from client %d.%d.%d.%d:%d\n",
					sender.GetA(), sender.GetB(), sender.GetC(), sender.GetD(), sender.GetPort() );
			recipients[sender].state = Connected;

			// Let packetHandler handle connection event
			if ( packetHandler ) {
				packetHandler->Connect( sender );
			}
		}
	}

	// Client-sided connection
	if ( sender == address ) {
		if ( mode == Client && state == Connecting ) {
			printf( "Network: client completes connection with server\n" );
			state = Connected;

			// Let packetHandler handle connection event
			if ( packetHandler ) {
				packetHandler->Connect( sender );
			}
		}
		timeoutAccumulator = 0.0f;
		return PACKET_SIZE;
	}
	return bytes_read;
}

void kl::Connection::ClearPackets() {
	for ( auto &pair : recipients ) {
		pair.second.packet.Clear();
	}
}
