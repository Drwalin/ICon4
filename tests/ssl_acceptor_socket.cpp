
#include <asio.hpp>
#include <socket.hpp>
#include <acceptor.hpp>

#include <thread>

const uint16_t PORT = 8437;

bool on_error_socket(net::socket* socket, const boost::system::error_code& err) {
	printf(" Error with socket: '%s'\n", err.message().c_str());
	if(err == boost::asio::error::bad_descriptor) {
		return false;
	}
	if(err == boost::asio::error::eof) {
		net::stop();
		return false;
	}
	return true;
}

bool on_error_acceptor(net::acceptor* acceptor,
		const boost::system::error_code& err) {
	printf(" Error with acceptor: '%s'\n", err.message().c_str());
	if(err == boost::asio::error::eof) {
		net::stop();
		return false;
	}
	return true;
}

void on_accept(net::acceptor* acceptor, net::socket* socket) {
	printf(" Accepted new socket!\n");
	socket->set_on_error(on_error_socket);
	socket->set_on_receive([](net::socket*socket, void*data, size_t bytes)
			{
				printf(" received from client: '");
				fwrite(data, 1, bytes, stdout);
				printf("' end received\n");
				std::string response = "server response!!";
				socket->send(response.data(), response.size());
				socket->close();
			});
}

void server() {
	net::error_code err;
	net::acceptor* acceptor = net::acceptor::make_ssl(err,
			net::endpoint::make_local_tcp(PORT), false, "cert/user.crt",
			"cert/user.key", "cert/dh2048.pem", "password2");
	acceptor->set_on_accept(on_accept);
	acceptor->set_on_error(on_error_acceptor);
	acceptor->start_listening();
	net::run();
}

int main() {
	std::thread server_thread(server);
	
	net::endpoint endpoint = net::endpoint::make_tcp("::1", PORT);
	net::error_code err;
	net::socket* socket = net::socket::make_ssl(err, endpoint, false, "root_ca.cert");
	if(socket) {
		socket->set_on_error(on_error_socket);
		std::string msg = "Text sent from client!!";
		
		socket->send(msg.data(), msg.length());
 		socket->set_on_receive(
				[](net::socket*sock, void*buf, size_t bytes) {
					printf(" received from server: '");
					fwrite(buf, 1, bytes, stdout);
					printf("' end received\n");
				});
	} else {
		printf(" Cannot connect to endpoint: %s\n", endpoint.to_string().c_str());
	}
	server_thread.join();
	return 0;
}

