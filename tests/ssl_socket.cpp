
#include <asio.hpp>
#include <chrono>
#include <mutex>
#include <socket.hpp>
#include <cstring>

#include <thread>

const uint16_t PORT = 8437;

bool single = true;


#include <atomic>
class _mutex {
public:
	_mutex() {
		flag = 1;
	}
	void lock() {
		int False = 0;
		while(flag.compare_exchange_weak(False, 1)) {
		}
	}
	void unlock() {
		flag = 0;
	}
	std::atomic<int> flag;
} __mutex;

class MAP {
public:
	MAP() {
		size = 0;
		bytes = 0;
		capacity = 0;
		entries = NULL;
	}
	struct entry {
		void* ptr;
		size_t size;
	};
	
	void ADD(void* ptr, size_t size) {
		__mutex.lock();
		grow();
		entries[this->size] = entry{ptr, size};
		++(this->size);
		bytes += size;
		__mutex.unlock();
	}
	void DELETE(void* ptr) {
		__mutex.lock();
		for(size_t i=0; i<size; ++i) {
			if(entries[i].ptr == ptr) {
				bytes -= entries[i].size;
				size--;
				/*
				memmove(&(entries[i]), &(entries[i+1]),
						sizeof(entry)*(size-i));
						*/
				entries[i] = entries[size];
				break;
			}
		}
		__mutex.unlock();
	}
	size_t allocated() const {
		return bytes + capacity*sizeof(entry);
	}
	void grow() {
		if(entries == NULL) {
			capacity = 16;
			entries = (entry*)malloc(capacity*sizeof(entry));
		} else if(size+5 >= capacity) {
			capacity = ((capacity*3)/2) + 3;
			entries = (entry*)realloc(entries, capacity*sizeof(entry));
		}
	}
	void PRINT_ENDS() {
		printf("\n");
		for(size_t i=0; i<size; ++i) {
			if(i<24 || i >= size-24)
				printf(" %16p -> %8lu\n", entries[i].ptr, entries[i].size);
		}
		printf(" c,s,b : %lu, %lu, %lu\n", capacity, size, bytes);
	}
	void PRINT() {
		printf("\n");
		for(size_t i=0; i<size; ++i) {
			printf(" %16p -> %8lu\n", entries[i].ptr, entries[i].size);
		}
		printf(" c,s,b : %lu, %lu, %lu\n", capacity, size, bytes);
	}
	
public:
	
	entry* entries;
	size_t capacity, size;
	size_t bytes;
} __map;

std::mutex mutex;
std::vector<net::socket*> socket_to_kill;
#define LOCK() std::lock_guard<std::mutex> ___lock(mutex)

bool on_error_socket(net::socket* socket, const boost::system::error_code& err) {
	if(single)
		printf(" Error with socket: '%s'\n", err.message().c_str());
	if(err == boost::asio::error::bad_descriptor ||
			err == boost::asio::error::connection_reset) {
// 		try {
			socket->cancel();
			LOCK();
			socket_to_kill.emplace_back(socket);
// 			net::stop();
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
// 			net::stop();
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
// 			net::stop();
// 		} catch(std::exception& e) {
// 			fprintf(stderr, " exception 3: %s\n", e.what());
// 		}
		return false;
	}
	return true;
}

std::atomic<size_t> deleted = 0;

void on_receive(net::socket*sock, void*buf, size_t bytes) {
	if(single) {
		printf(" received from server: '");
		fwrite(buf, 1, bytes, stdout);
		printf("' end received\n");
		single = false;
	}
	sock->close();
	LOCK();
	socket_to_kill.emplace_back(sock);
// 	net::stop();
//	sock->send("fdsafdsafegfgfgeg", 13);
}

std::string msg = "Text sent from client!!";
net::socket* init_new_client() {
	net::endpoint endpoint = net::endpoint::make_tcp("::1", PORT);
	net::error_code err;
	net::socket* socket = net::socket::make_tcp(err, endpoint, false);
	if(socket) {
		socket->set_on_error(on_error_socket);
		socket->send(msg.data(), msg.length());
		socket->set_on_receive(on_receive);
	}
	return socket;
}

size_t multi_client(size_t count) {
	size_t res = 0;
	for(size_t i=0; i<count; ++i) {
		res += init_new_client() ? 1 : 0;
	}
 	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	LOCK();
	for(net::socket* sock : socket_to_kill) {
		sock->close();
		delete sock;
		++deleted;
	}
	socket_to_kill.clear();
	return res;
}

void client() {
	init_new_client();
	LOCK();
	for(net::socket* sock : socket_to_kill) {
		sock->close();
		delete sock;
		++deleted;
	}
	socket_to_kill.clear();
}
/*
void client() {
	net::endpoint endpoint = net::endpoint::make_tcp("::1", PORT);
	net::error_code err;
	net::socket* socket = net::socket::make_tcp(err, endpoint, false);
	if(socket) {
		socket->set_on_error(on_error_socket);
		std::string msg = "Text sent from client!!";

// 		try {
			socket->send(msg.data(), msg.length());
			socket->set_on_receive(on_receive);
// 		} catch(std::exception& e) {
// 			fprintf(stderr, " exception 4: %s\n", e.what());
// 		}
// 		try {
// 		} catch(std::exception& e) {
// 			fprintf(stderr, " exception 5: %s\n", e.what());
// 		}
// 		try {
 			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			socket->cancel();
// 			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			socket->close();
// 			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			delete socket;
			++deleted;
// 		} catch(std::exception& e) {
// 			fprintf(stderr, " exception 6: %s\n", e.what());
// 		}
	} else if(single) {
		printf(" Cannot connect to endpoint: %s\n", endpoint.to_string().c_str());
	}
}

size_t multi_client(size_t count) {
	std::string msg = "Text sent from client!!";
	net::endpoint endpoint = net::endpoint::make_tcp("::1", PORT);
	net::error_code err;
	net::socket* sockets[count];
	size_t ret = 0;
	for(size_t i=0; i<count; ++i) {
		net::socket* socket = net::socket::make_tcp(err, endpoint, false);
		sockets[i] = socket;
		if(socket) {
			socket->send(msg.data(), msg.length());
			socket->set_on_receive(on_receive);
			ret++;
		}
	}
	
 	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	
	for(size_t i=0; i<count; ++i) {
		net::socket* socket = sockets[i];
		if(socket) {
// 			socket->cancel();
// 			socket->close();
			delete socket;
			++deleted;
		}
	}
	
	return ret;
}

*/
#include <ctime>

void run() {
	net::run();
}

int main() {
// 	try {
		std::thread runner(run);
		client();
		single = false;
		size_t beg = clock();
		size_t BEG = beg;
		printf("\n done: 0 in 0s");
		fflush(stdout);
		size_t i=2;
		size_t j=0;
		for(; j<10; ++i, ++j) {
			net::reset();
// 			try {
			const int C=171;
			const bool use_thread = true;
			if(!use_thread) {
				client();
			} else {
				i += multi_client(C) - 1;
// 				std::thread t[C];
// 				for(int I=0; I<C; ++I)
// 					t[I] = std::thread(client);
// 				i += C-1;
// 				for(int I=0; I<C; ++I)
// 					t[I].join();
			}
// 			} catch(std::exception& e) {
// 				fprintf(stderr, " exception 7: %s\n", e.what());
// 			}
			if(j%(1027/(use_thread?C:1)) == 0) {
				__map.PRINT_ENDS();
				printf("\r done: %8lu/%8lu in %2.2fs   alloc [%8lu b] %lu     ",
						deleted.load(), i,
						(float(clock()-BEG))/(float)CLOCKS_PER_SEC,
						__map.allocated(), __map.size);
				fflush(stdout);
			}
			if((clock()-beg)*5 > CLOCKS_PER_SEC) {
				beg = clock();
			}
		}
		printf("\r done: %8lu/%8lu in %2.2fs", deleted.load(), i,
				(float(clock()-BEG))/(float)CLOCKS_PER_SEC);
		fflush(stdout);
		runner.join();
// 	} catch(std::exception& e) {
// 		fprintf(stderr, " exception 7: %s\n", e.what());
// 	}
	return 0;
}


void* operator new(size_t size) {
// 	size += 1024 - (size&1023);
	void* ptr = malloc(size);
	__map.ADD(ptr, size);
	return ptr;
}

void operator delete(void* ptr) {
	__map.DELETE(ptr);
	free(ptr);
}

