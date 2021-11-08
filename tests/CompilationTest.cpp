
#include <Socket.hpp>
#include <ASIO.hpp>
#include <Listener.hpp>

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <thread>
#include <mutex>
#include <exception>

std::atomic<bool> running = true;

const static uint16_t SERVER_PORT = 12345;

std::mutex mutex;
void OnError(const boost::system::error_code& err) {
	std::lock_guard<std::mutex> lock(mutex);
	printf(" Error: '%s'\n", err.message().c_str());
	fflush(stdout);
}

void OnErrorSocket(Socket* socket, const boost::system::error_code& err) {
	std::lock_guard<std::mutex> lock(mutex);
	printf(" Error with socket: '%s'\n", err.message().c_str());
	fflush(stdout);
}

Socket *serSock;

bool OnAccept(Socket* socket) {
	printf(" Accepting socket\n");
	serSock = socket;
	socket->SetOnError(OnErrorSocket);
	socket->SetOnReceive([](Socket*socket, void*buffer, size_t bytes){
			std::lock_guard<std::mutex> lock(mutex);
			printf(" Received from client: [%ld] '%s'\n", bytes,
					(const char*)buffer);
			fflush(stdout);
			});
	std::string message = "A message sent from server!";
	bool res = socket->Send(message.data(), message.size()+1);
	if(res == false) {
		std::lock_guard<std::mutex> lock(mutex);
		printf(" Send from server failed\n");
		fflush(stdout);
	}
	return true;
}

void server() {
	try {
		Listener listener;
		listener.SetOnError(OnError);
		Endpoint endpoint(
				"127.0.0.1",
				SERVER_PORT);
		endpoint.Port(SERVER_PORT);
		boost::system::error_code err;
		listener.StartAccepting(endpoint, OnAccept, true, err);
		if(err) {
			OnError(err);
			return;
		}
	} catch(std::exception &e) {
		std::lock_guard<std::mutex> lock(mutex);
		printf(" Exception server: %s", e.what());
		fflush(stdout);
	}
}

void client() {
	try {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		boost::system::error_code err;
		Socket socket(Endpoint("127.0.0.1", SERVER_PORT), err, true);
		if(err) {
			OnErrorSocket(NULL, err);
			return;
		}
		socket.SetOnError(OnErrorSocket);
		socket.SetOnReceive([](Socket*socket, void*buffer, size_t bytes){
				std::lock_guard<std::mutex> lock(mutex);
				printf(" Received from server: [%ld] '%s'\n", bytes,
						(const char*)buffer);
				fflush(stdout);
				});

		std::string message = "A message sent from client!";
		bool res = socket.Send(message.data(), message.size()+1);
		if(res == false) {
			std::lock_guard<std::mutex> lock(mutex);
			printf(" Send from client failed\n");
			fflush(stdout);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	} catch(std::exception &e) {
		std::lock_guard<std::mutex> lock(mutex);
		printf(" Exception client: %s", e.what());
		fflush(stdout);
	}
}

void IoPoll() {
	while(running) {
		try{
			IoContextPollOne();
			std::this_thread::yield();
		} catch(std::exception &e) {
			std::lock_guard<std::mutex> lock(mutex);
			printf(" Exception io poll: %s", e.what());
			fflush(stdout);
		}
	}
}

int main() {
	std::thread io(IoPoll);
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	std::thread s(server), c(client);
	s.join();
	c.join();
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	running = false;
	io.join();
	delete serSock;
	return 0;
}

