
#include <asio.hpp>
#include <socket.hpp>
#include <acceptor.hpp>

const uint16_t PORT = 8437;

std::mutex mutex;
std::vector<net::socket*> socket_to_kill;
#define LOCK() std::lock_guard<std::mutex> ___lock(mutex)
#include <atomic>

std::atomic<size_t> sockets_count = 0;

bool on_error_socket(net::socket* socket, const boost::system::error_code& err) {
// 	printf(" Error with socket: '%s'\n", err.message().c_str());
	if(err == boost::asio::error::bad_descriptor ||
			err == boost::asio::error::connection_reset) {
// 		try {
			socket->cancel();
			LOCK();
			socket_to_kill.emplace_back(socket);
// 		} catch(std::exception& e) {
// 			fprintf(stderr, " exception 1: %s\n", e.what());
// 		}
		return false;
	}
	if(err == boost::asio::error::eof) {
// 		try {
			socket->cancel();
			LOCK();
			socket_to_kill.emplace_back(socket);
// 		} catch(std::exception& e) {
// 			fprintf(stderr, " exception 2: %s\n", e.what());
// 		}
		return false;
	}
	if(err == boost::asio::ssl::error::stream_truncated) {
// 		try {
			socket->cancel();
			LOCK();
			socket_to_kill.emplace_back(socket);
// 		} catch(std::exception& e) {
// 			fprintf(stderr, " exception 3: %s\n", e.what());
// 		}
		return false;
	}
	return true;
}

bool on_error_acceptor(net::acceptor* acceptor,
		const boost::system::error_code& err) {
	printf(" Error with acceptor: '%s', sockets count: %lu\n", err.message().c_str(), sockets_count.load());
	if(err == boost::asio::error::eof) {
// 		try {
			net::stop();
// 		} catch(std::exception& e) {
// 			fprintf(stderr, " exception 4: %s\n", e.what());
// 		}
		return false;
	}
	return true;
}

void on_accept(net::acceptor* acceptor, net::socket* socket) {
	printf("new socket");
	{
			LOCK();
	for(net::socket* sock : socket_to_kill) {
// 		try {
// 			printf(" delete ...\n");
			if(sock == socket)
				socket = NULL;
			sock->close();
			delete sock;
			--sockets_count;
// 		} catch(std::exception& e) {
// 			fprintf(stderr, " exception 5: %s\n", e.what());
// 		}
	}
	socket_to_kill.clear();
	}
	if(socket == NULL)
		return;
// 	printf(" Accepted new socket!\n");
	++sockets_count;
	socket->set_on_error(on_error_socket);
	socket->set_on_receive([](net::socket*socket, void*data, size_t bytes)
			{
// 			printf(" received from client: '");
// 			fwrite(data, 1, bytes, stdout);
// 			printf("' end received\n");
			std::string response = "server response!!";
			socket->send(response.data(), response.size());
			//socket->close();
			});
}

int main() {
	net::error_code err;
	net::acceptor* acceptor = net::acceptor::make_tcp(err,
			net::endpoint::make_local_tcp(PORT), false);//, "cert/user.crt",
// 			"cert/user.key", "cert/dh2048.pem", "password2");
	acceptor->set_on_accept(on_accept);
	acceptor->set_on_error(on_error_acceptor);
	acceptor->start_listening();
// 	try {
		net::run();
// 	} catch(std::exception& e) {
// 		fprintf(stderr, " exception 6: %s\n", e.what());
// 	}
	return 0;
}


