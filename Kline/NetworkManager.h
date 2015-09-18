#pragma once

#define PACKET_SIZE 1024

namespace kl {

class WritePacket {
public:
	WritePacket() {
		Clear();
	}

	int Size() const {
		return writePos;
	}

	void Clear() {
		memset( buffer, 0, PACKET_SIZE );
		writePos = 0;
		WriteHeader();
	}

	const char *Data() const {
		return buffer;
	}

	template <typename T>
	void WriteNative( const T &value ) {
		// Prevent overflow
		assert( writePos + sizeof( T ) <= PACKET_SIZE );

		// Convert T to char array
		union {
			T value;
			char data[sizeof( T )];
		} c;
		c.value = value;

		// Write data
		memcpy( buffer + writePos, c.data, sizeof( T ) );

		// Increment write position
		writePos += sizeof( T );
	}

	void WriteString( const char *msg ) {
		int length = ( int ) strlen( msg ) + 1;
		assert( writePos + length <= PACKET_SIZE );
		// Write data
		memcpy( buffer + writePos, msg, length );
		writePos += length;
	}

	void WriteHeader();

	void WriteU8( uint8_t value ) {
		WriteNative( value );
	}

	void WriteU16( uint16_t value ) {
		WriteNative( htons( value ) );
	}

	void WriteU32( uint32_t value ) {
		WriteNative( htonl( value ) );
	}

	void WriteU64( uint64_t value ) {
		WriteNative( htonll( value ) );
	}

	void WriteFloat( float value ) {
		WriteNative( htonf( value ) );
	}

	void WriteVec3( const glm::vec3 &vec ) {
		WriteFloat( vec.x );
		WriteFloat( vec.y );
		WriteFloat( vec.z );
	}

private:

	char buffer[PACKET_SIZE];
	int writePos;
};

class ReadPacket {
public:
	ReadPacket() {
		Clear();
	}

	void SetSize( int pos ) {
		this->size = pos;
	}

	size_t GetSize() const {
		return size;
	}

	size_t GetReadPos() const {
		return readPos;
	}

	void Clear() {
		memset( buffer, 0, PACKET_SIZE );
		size = 0;
		readPos = 0;
	}

	char *DataWritable() {
		return buffer;
	}

	/************************************************************************/
	/* Read                                                                 */
	// FIXME: Perhaps pass out variable by reference and return bool for success?
	/************************************************************************/
	uint8_t ReadU8() {
		return ReadNative<uint8_t>();
	}

	uint16_t ReadU16() {
		return ntohs( ReadNative<uint16_t>() );
	}

	uint32_t ReadU32() {
		return ntohl( ReadNative<uint32_t>() );
	}

	uint64_t ReadU64() {
		return ntohll( ReadNative<uint64_t>() );
	}

	float ReadFloat() {
		return ntohf( ReadNative<uint32_t>() );
	}

	void ReadString( char *dest ) {
		// strlen does not count \0, so we need to add 1 every time
		int length = ( int ) strlen( buffer + readPos ) + 1;
		memcpy( dest, buffer + readPos, length );
		readPos += length;
	}

	glm::vec2 ReadVec2() {
		float x = ReadFloat();
		float y = ReadFloat();
		return glm::vec2( x, y );
	}

	glm::vec3 ReadVec3() {
		float x = ReadFloat();
		float y = ReadFloat();
		float z = ReadFloat();
		return glm::vec3( x, y, z );
	}

	template <typename T>
	T ReadNative() {
		union coercion {
			T value;
			char data[sizeof( T )];
		};

		coercion c;

		memcpy( &c.data, buffer + readPos, sizeof( T ) );
		readPos += sizeof( T );

		return c.value;
	}

	void Print() {
		uint8_t *buf = ( uint8_t * ) buffer;
		for ( int i = 0; i < size; i++ ) {
			printf( "%02X ", buf[i] );
		}
		printf( "\n" );
	}

private:
	char buffer[PACKET_SIZE];
	int readPos; // Number of bytes already read
	int size; // Size of contents
};

class Address {
public:

	Address() :
		address( 0 ),
		port( 0 ) {}

#if 0
	Address( const char *address, unsigned short port ) {
		unsigned long addr;
		inet_pton( AF_INET, address, &addr );
		this->address = htonl( addr );
		this->port = port;
	}
#endif

	Address( unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port ) :
		address( ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d ),
		port( port ) {}

	Address( unsigned int address, unsigned short port ) :
		address( address ),
		port( port ) {}

	unsigned int GetAddress() const {
		return address;
	}

	unsigned char GetA() const {
		return static_cast<uint8_t>( address >> 24 );
	}

	unsigned char GetB() const {
		return static_cast<uint8_t>( address >> 16 );
	}

	unsigned char GetC() const {
		return static_cast<uint8_t>( address >> 8 );
	}

	unsigned char GetD() const {
		return static_cast<uint8_t>( address );
	}

	unsigned short GetPort() const {
		return port;
	}

	bool operator == ( const Address &other ) const {
		return address == other.address && port == other.port;
	}

	bool operator != ( const Address &other ) const {
		return !( *this == other );
	}

	bool operator < ( const Address &other ) const {
		// note: this is so we can use address as a key in std::map
		if ( address < other.address )
			return true;
		if ( address > other.address )
			return false;
		else
			return port < other.port;
	}

private:

	unsigned int address;
	unsigned short port;
};

// RAII implementation of a UDP socket
class Socket {
public:

	Socket() {
		socket = 0;
	}

	~Socket() {
		Close();
	}

	bool Open( unsigned short port ) {
		assert( !IsOpen() );

		// create socket

		// ::socket is a WinSock2 function
		socket = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

		if ( socket <= 0 ) {
			printf( "Network: failed to create socket\n" );
			socket = 0;
			return false;
		}

		// bind to port

		sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons( port );

		if ( bind( socket, ( const sockaddr * ) &address, sizeof( sockaddr_in ) ) < 0 ) {
			printf( "Network: failed to bind socket\n" );
			Close();
			return false;
		}

		// set non-blocking io
		DWORD nonBlocking = 1;
		if ( ioctlsocket( socket, FIONBIO, &nonBlocking ) != 0 ) {
			printf( "Network: failed to set non-blocking socket\n" );
			Close();
			return false;
		}

		return true;
	}

	void Close() {
		if ( socket != 0 ) {
			closesocket( socket );
			socket = 0;
		}
	}

	bool IsOpen() const {
		return socket != 0;
	}

	bool Send( const Address &destination, const WritePacket &packet );

	int Receive( Address &sender, ReadPacket &packet ) {
		if ( socket == 0 )
			return false;

		sockaddr_in from;
		socklen_t fromLength = sizeof( from );

		int received_bytes = recvfrom( socket, packet.DataWritable(), PACKET_SIZE, 0, ( sockaddr * ) &from, &fromLength );
		packet.SetSize( received_bytes );

		if ( received_bytes <= 0 )
			return 0;

		unsigned int address = ntohl( from.sin_addr.s_addr );
		unsigned short port = ntohs( from.sin_port );

		sender = Address( address, port );

		return received_bytes;
	}

private:
	SOCKET socket;
};

class PacketHandler;

class Connection {
public:

	enum Mode {
		None,
		Client,
		Server
	};

	Connection() {}

	Connection( unsigned int protocolId, float timeout ) {
		this->protocolId = protocolId;
		this->timeout = timeout;
		mode = None;
		running = false;
		ClearData();
	}

	~Connection() {
		if ( running )
			Stop();
	}

	bool Start( unsigned short port ) {
		if ( running ) {
			Stop();
		}
		if ( !socket.Open( port ) )
			return false;
		running = true;
		return true;
	}

	void Stop() {
		assert( running );
		printf( "Network: stop connection\n" );
		mode = None;
		ClearData();
		socket.Close();
		running = false;
	}

	void Listen() {
		ClearData();
		mode = Server;
		state = Listening;
	}

	void Connect( const Address &address ) {
		printf( "Network: client connecting to %d.%d.%d.%d:%d\n",
				address.GetA(), address.GetB(), address.GetC(), address.GetD(), address.GetPort() );
		ClearData();
		mode = Client;
		state = Connecting;
		this->address = address;
		recipients[address] = ClientInfo();
	}

	bool IsConnecting() const {
		return state == Connecting;
	}

	bool ConnectFailed() const {
		return state == ConnectFail;
	}

	bool IsConnected() const {
		return state == Connected;
	}

	bool IsListening() const {
		return state == Listening;
	}

	Mode GetMode() const {
		return mode;
	}

	void Update( float deltaTime );

	bool SendPackets() {
		switch ( mode ) {
		case Client:
		case Server: {
			bool success = false;
			if ( running ) {
				for ( auto &pair : recipients ) {
					if ( socket.Send( pair.first, pair.second.packet ) ) {
						success = true;
					}
				}
			} else return false;
			return success;
			break;
		}
		default:
			return false;
			break;
		}
	}

	int ReceivePacket( Address &sender, ReadPacket &packet );

	void SetPacketHandler( PacketHandler *handler ) {
		this->packetHandler = handler;
	}

	void WriteString( const Address &address, const char *msg ) {
		recipients[address].packet.WriteString( msg );
	}

	template <typename T>
	void WriteNative( const Address &address, const T &value ) {
		recipients[address].packet.WriteNative( value );
	}

	void WriteString( const char *msg ) {
		for ( auto &pair : recipients ) {
			pair.second.packet.WriteString( msg );
		}
	}

	template <typename T>
	void WriteNative( const T &value ) {
		for ( auto &pair : recipients ) {
			pair.second.packet.WriteNative( value );
		}
	}

	void ClearPackets();
protected:

	void ClearData() {
		state = Disconnected;
		timeoutAccumulator = 0.0f;
		address = Address();
#if 1
		recipients.clear();
#endif
	}
private:

	enum State {
		Disconnected,
		Listening,
		Connecting,
		ConnectFail,
		Connected
	};

	struct ClientInfo {
		State state = Disconnected;
		float timeoutAccumulator = 0.0f;
		WritePacket packet;
	};

	unsigned int protocolId;
	float timeout;

	bool running;
	Mode mode;
	State state;
	Socket socket;

	float timeoutAccumulator;
	Address address;
	std::map<Address, ClientInfo> recipients;
	PacketHandler *packetHandler = nullptr;
}; // !class Connection

class PacketHandler;
class NetworkManager {
public:
	static void Init();
	static void Update();
	static void Destroy();

	static void StartServer();
	static void StartClient( const char *address );
	static Connection::Mode GetMode() { return connection.GetMode(); }
	static bool IsServer();
	static void SetPacketHandler( PacketHandler *handler ) { packetHandler = handler; connection.SetPacketHandler( handler ); }
	static void Send();
	static bool IsConnected() { return connection.IsConnected(); }

	/************************************************************************/
	/* Write                                                                */
	/************************************************************************/
	static void WriteU8( const Address &address, uint8_t value ) {
		connection.WriteNative( address, value );
	}

	static void WriteU16( const Address &address, uint16_t value ) {
		connection.WriteNative( address, htons( value ) );
	}

	static void WriteU32( const Address &address, uint32_t value ) {
		connection.WriteNative( address, htonl( value ) );
	}

	static void WriteU64( const Address &address, uint64_t value ) {
		connection.WriteNative( address, htonll( value ) );
	}

	static void WriteFloat( const Address &address, float value ) {
		connection.WriteNative( address, htonf( value ) );
	}

	static void WriteString( const Address &address, const char *msg ) {
		connection.WriteString( address, msg );
	}

	static void WriteVec3( const Address &address, const glm::vec3 &vec ) {
		WriteFloat( address, vec.x );
		WriteFloat( address, vec.y );
		WriteFloat( address, vec.z );
	}

	static void WriteVec2( const Address &address, const glm::vec2 &vec ) {
		WriteFloat( address, vec.x );
		WriteFloat( address, vec.y );
	}

	/************************************************************************/
	/* BROADCAST                                                            */
	/************************************************************************/

	static void BroadcastU8( uint8_t value ) {
		connection.WriteNative( value );
	}

	static void BroadcastU16( uint16_t value ) {
		connection.WriteNative( htons( value ) );
	}

	static void BroadcastU32( uint32_t value ) {
		connection.WriteNative( htonl( value ) );
	}

	static void BroadcastU64( uint64_t value ) {
		connection.WriteNative( htonll( value ) );
	}

	static void BroadcastFloat( float value ) {
		connection.WriteNative( htonf( value ) );
	}

	static void BroadcastString( const char *msg ) {
		connection.WriteString( msg );
	}

	static void BroadcastVec3( const glm::vec3 &vec ) {
		BroadcastFloat( vec.x );
		BroadcastFloat( vec.y );
		BroadcastFloat( vec.z );
	}

	static void BroadcastVec2( const glm::vec2 &vec ) {
		BroadcastFloat( vec.x );
		BroadcastFloat( vec.y );
	}

	static void ResetCounters() {
		numBytesSentInclusive = 0;
		numBytesSentExclusive = 0;
	}

	static void IncrementBytesSentInclusive(int numBytes) {
		numBytesSentInclusive += numBytes;
	}

	static void IncrementBytesSentExclusive(int numBytes) {
		numBytesSentExclusive += numBytes;
	}
	static void PrintCounters() {
		printf("numBytesSentInclusive: %d\n", numBytesSentInclusive);
		printf("numBytesSentExclusive: %d\n", numBytesSentExclusive);
	}
private:
	static bool shouldSend;
	static float time;
	static Connection connection;
	static bool connected;
	static PacketHandler *packetHandler;

	static int64_t numBytesSentInclusive;
	static int64_t numBytesSentExclusive;
};
}
