
#include <boost/asio/error.hpp>
#include <socket.hpp>
#include <acceptor.hpp>
#include <asio.hpp>

bool on_error(net::socket* socket, const boost::system::error_code& err) {
	printf(" Error with socket: '%s'\n", err.message().c_str());
	if(err == boost::asio::error::bad_descriptor) {
		return false;
	}
	if(err == boost::asio::error::eof) {
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
	socket->set_on_error(on_error);
	socket->set_on_receive([](net::socket*socket, void*data, size_t bytes)
			{
			printf(" received: ");
			fwrite(data, 1, bytes, stdout);
			if(std::string((char*)data, bytes).find("\n\r\n") != std::string::npos) {
				socket->close();
			}
			});
	std::string msg = "{'taki_tam': 'kod'}";
	socket->send(msg.data(), msg.size());
}

int main() {
	net::error_code err;
	net::acceptor* acceptor = net::acceptor::make_tcp(err,
			net::endpoint::make_local_tcp(8080), false);
	acceptor->set_on_accept(on_accept);
	acceptor->set_on_error(on_error_acceptor);
	acceptor->start_listening();
	
	net::run();
	
	return 0;
}

