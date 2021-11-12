
include ../MakefileSTD/MakefileSTD


CXX = g++
CXXFLAGS = -Ofast -pipe -std=c++20 -pedantic -Wall
LIBS=

ifeq ($(platform),win)
	LIBS += -lzlib
	LIBS += -lgcc_s_seh-1 -lmfc100
	LIBS += -lmfc100u -lmsvcp100 -lmsvcr100 -lmsvcr100_clr0400
else
	LIBS += -L/usr/lib -ldl -lz
	CXXFLAGS += -fPIC
	CXXFLAGS += -I/usr/include
endif
LIBS += -lpthread -lboost_system -lssl -lcrypto

INCLUDE = -I../src/
CXXFLAGS += $(INCLUDE)
OBJECTS =bin/asio.o bin/socket.o bin/acceptor.o
OBJECTS+=bin/tcp_socket_impl.o bin/tcp_acceptor_impl.o
OBJECTS+=bin/ssl_socket_impl.o bin/ssl_acceptor_impl.o

EXE = tcp_socket.exe tcp_acceptor.exe ssl_acceptor_socket.exe ssl_acceptor.exe ssl_socket.exe
EXE_OBJ = bin/tcp_socket.o bin/tcp_acceptor.o bin/ssl_acceptor_socket.o bin/ssl_acceptor.o bin/ssl_socket.o

all: $(EXE_OBJ) $(OBJECTS) $(EXE)

%.exe: bin/%.o $(OBJECTS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

bin/%.o: ../src/%.cpp ../src/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

bin/%.o: %.cpp
	$(CXX) -c -o $@ $^ $(CXXFLAGS)

.PHONY: clean
clean:
	$(RM) *.exe *.o bin/*.o

