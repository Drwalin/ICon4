
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
} mutex;

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
		mutex.lock();
		grow();
		entries[this->size] = entry{ptr, size};
		++(this->size);
		bytes += size;
		mutex.unlock();
	}
	void DELETE(void* ptr) {
		mutex.lock();
		for(size_t i=0; i<size; ++i) {
			if(entries[i].ptr == ptr) {
				bytes -= entries[i].size;
				size--;
				memmove(entries+i, entries+i+1, sizeof(entry)*(size-i));
			}
		}
		mutex.unlock();
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

bool on_error_socket(net::socket* socket, const boost::system::error_code& err) {
	if(single)
		printf(" Error with socket: '%s'\n", err.message().c_str());
	if(err == boost::asio::error::bad_descriptor ||
			err == boost::asio::error::connection_reset) {
// 		try {
			socket->cancel();
			net::stop();
// 		} catch(std::exception& e) {
// 			fprintf(stderr, " exception 1: %s\n", e.what());
// 		}
		return false;
	}
	if(err == boost::asio::error::eof) {
// 		try {
			socket->cancel();
			net::stop();
// 		} catch(std::exception& e) {
// 			fprintf(stderr, " exception 2: %s\n", e.what());
// 		}
		return false;
	}
	if(err == boost::asio::ssl::error::stream_truncated) {
// 		try {
			socket->cancel();
			net::stop();
// 		} catch(std::exception& e) {
// 			fprintf(stderr, " exception 3: %s\n", e.what());
// 		}
		return false;
	}
	return true;
}

size_t deleted = 0;

void on_receive(net::socket*sock, void*buf, size_t bytes) {
	if(single) {
		printf(" received from server: '");
		fwrite(buf, 1, bytes, stdout);
		printf("' end received\n");
		single = false;
	}
	net::stop();
//	sock->send("fdsafdsafegfgfgeg", 13);
}

void client() {
	net::endpoint endpoint = net::endpoint::make_tcp("::1", PORT);
	net::error_code err;
	net::socket* socket = net::socket::make_tcp(err, endpoint, false);
	if(socket) {
		socket->set_on_error(on_error_socket);
		std::string msg = "Text sent from client!!";

// 		try {
			socket->send(msg.data(), msg.length());
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			socket->set_on_receive(on_receive);
// 		} catch(std::exception& e) {
// 			fprintf(stderr, " exception 4: %s\n", e.what());
// 		}
// 		try {
			net::run();
// 		} catch(std::exception& e) {
// 			fprintf(stderr, " exception 5: %s\n", e.what());
// 		}
// 		try {
			socket->cancel();
			socket->close();
			delete socket;
			++deleted;
// 		} catch(std::exception& e) {
// 			fprintf(stderr, " exception 6: %s\n", e.what());
// 		}
	} else if(single) {
		printf(" Cannot connect to endpoint: %s\n", endpoint.to_string().c_str());
	}
}

#include <ctime>

int main() {
	try {
		client();
		single = false;
		size_t beg = clock();
		size_t BEG = beg;
		printf("\n done: 0 in 0s");
		fflush(stdout);
		size_t i=2;
		for(; ; ++i) {
// 			try {
				client();
// 			} catch(std::exception& e) {
// 				fprintf(stderr, " exception 7: %s\n", e.what());
// 			}
			if(i%1237 == 0) {
				__map.PRINT();
				printf("\r done: %8lu/%8lu in %2.2fs   alloc [%8lu b] %lu     ",
						deleted, i,
						(float(clock()-BEG))/(float)CLOCKS_PER_SEC,
						__map.allocated(), __map.size);
				fflush(stdout);
			}
			if((clock()-beg)*5 > CLOCKS_PER_SEC) {
				beg = clock();
			}
		}
		printf("\r done: %8lu/%8lu in %2.2fs", deleted, i,
				(float(clock()-BEG))/(float)CLOCKS_PER_SEC);
		fflush(stdout);
	} catch(std::exception& e) {
		fprintf(stderr, " exception 7: %s\n", e.what());
	}
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

