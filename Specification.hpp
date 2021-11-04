
using size_t = unsigned long long int;
using uint32_t = unsigned int;
using uint8_t = unsigned char;

namespace std {
	template<typename T>
	class vector {
	};
}

struct Endpoint {
};

enum ConnectionType {
	ConUDP,
	ConTCP,
	ConSSL,
	ConBroadcast
};

class UDP {
	bool Send(const void* data, size_t bytes);
	void QueueFetch(std::vector<uint8_t> buffer, ...);
};

class TCP {
	bool Send(const void* data, size_t bytes);
	void QueueFetch(std::vector<uint8_t> buffer, size_t offset, ...);
};

class SSL {
	bool Send(const void* data, size_t bytes);
	void QueueFetch(std::vector<uint8_t> buffer, size_t offset, ...);
};

class Broadcast {
	bool Send(const void* data, size_t bytes);
	void QueueFetch(std::vector<uint8_t> buffer, ...);
};

struct ConnectionData {
	std::vector<uint8_t> buffer;
	size_t fetchedBytes;
	ConnectionType connectionType;
	bool enableSizeInHeader;
	union {
		UDP udp;
		TCP tcp;
		SSL ssl;	
		Broadcast broadcast;
	} impl;
	
	void Fetch(struct Error, size_t receivedBytes);
};

class Connection {
public:
	Connection();
	~Connection();
	
	bool Connect(Endpoint, ConnectionType, bool enableSizeInHeader);
	bool ConnectSSL(Endpoint, bool enableSizeInHeader,
			const char* rootCertFile);
	void Disconnect();
	
	bool Send(const void* data, size_t bytes);
	
	void SetOnConnectionLost(void(*)(Connection*));
	void SetOnReceive(void(*)(Connection*, const void* data, size_t bytes));
	void SetOnDisconnect(void(*)(Connection*));
	
	void* GetUserPointer();
	void SetUserPointer(void*);
	const Endpoint& GetEndpoint() const;
};

class Server {
public:
	Server();
	~Server();
	
	bool Listen(Endpoint, ConnectionType, bool enableSizeInHeader);
	bool ListenSSL(Endpoint, bool enableSizeInHeader,
			const char* certFile, const char* keyFile, const char* dhFile);
	void DisconnectAll();
	
	bool Send(const void* data, size_t bytes);
	bool SendAll(const void* data, size_t bytes);
	
	void SetOnConnectionLost(void(*)(Connection*));
	void SetOnReceive(void(*)(Connection*, const void* data, size_t bytes));
	void SetOnDisconnect(void(*)(Connection*));
	
	void SetOnAccept(bool(*)(Connection*));
};

void NetPollEvents();

