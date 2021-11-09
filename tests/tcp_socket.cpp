
#include <socket.hpp>
#include <asio.hpp>

bool on_error(net::socket* socket, const boost::system::error_code& err) {
	printf(" Error with socket: '%s'\n", err.message().c_str());
	if(err == boost::asio::error::eof) {
		net::stop();
		return false;
	}
	return true;
}

int main() {
	net::endpoint endpoint = net::endpoint::resolve_any_tcp("example.com", "http");
	printf(" Resolved endpoint: %s\n", endpoint.to_string().c_str());
	net::error_code err;
	net::socket* socket = net::socket::make_tcp(err, endpoint, false);
	if(socket) {
		socket->set_on_error(on_error);
		std::string msg = "GET / HTTP/1.0\r\nHost: example.com\r\nAccept: */*\r\nConnection: close\r\n\r\n";
		
		socket->send(msg.data(), msg.length());
 		socket->set_on_receive(
				[](net::socket*sock, void*buf, size_t bytes) {
					fwrite(buf, 1, bytes, stdout);
				});
		
		net::run();
	} else {
		printf(" Cannot connect to endpoint: %s\n", endpoint.to_string().c_str());
	}
	return 0;
}

